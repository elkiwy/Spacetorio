#include "Universe.hpp"

#include "Components.hpp"
#include "Components_position.hpp"
#include "Components_renderables.hpp"
#include "Components_planet.hpp"
#include "Components_colliders.hpp"
#include "Components_clickables.hpp"

#include "Entity.hpp"
#include "Utils_math.hpp"

/*
** PlanetBiome
*/

PlanetBiome::PlanetBiome(Entity planetEntity, PlanetBiomeType t) {
    scene = planetEntity.scene; enttHandle = planetEntity.scene->newEntity();
    addComponent<TagComponent>("_UnnamedBiome_");
    addComponent<PlanetBiomeComponent>(t);
    auto& posComp = planetEntity.getComponent<DynamicPositionComponent>();
    auto& myPosComp = addComponent<DynamicPositionComponent>(posComp.pos);
    addComponent<ClickableCircleComponent>(100.0f, &myPosComp);
    //addComponent<RenderableCircleComponent>(size);
}


/*
** Planet
*/

Planet::Planet(Scene *s, std::string name, fPoint pos, float size) {
    scene = s; enttHandle = s->getRegistry().create();
    addComponent<TagComponent>((name.empty()) ? "_UnnamedPlanet_" : name);
    addComponent<DynamicPositionComponent>(pos);
    Entity myEntity = {enttHandle, scene};
    addComponent<PlanetComponent>(myEntity, size);
    //addComponent<RenderableCircleComponent>(size);
}



/*
** StarSystem
*/

StarSystem::StarSystem(Scene *s, std::string name, fPoint pos) {
    scene = s; enttHandle = s->getRegistry().create();
    addComponent<TagComponent>((name.empty()) ? "_UnnamedStarSystem_" : name);
    addComponent<DynamicPositionComponent>(pos);
    addComponent<StarSystemComponent>();
}

Planet StarSystem::addRandomStar() {
    fPoint pos = {0.0f, 0.0f};
    float size = randFloat(200.0f, 300.0f);
    std::string name = "Star " + std::to_string(randInt(1000, 9999));
    return Planet(scene, name, pos, size);
}

Planet StarSystem::addRandomPlanet() {
    fPoint pos = {0.0f, 0.0f}; //{randFloat(400.0f, 2000.0f), 0.0f};
    float size = 600.0f;//randFloat(100.0f, 500.0f);
    std::string name = "RandomPlanet " + std::to_string(randInt(1000, 9999));
    return Planet(scene, name, pos, size);
}



/*
** PlayerSpaceship
*/

PlayerSpaceship::PlayerSpaceship(Scene *s, fPoint pos) {
    scene = s; enttHandle = s->getRegistry().create();
    addComponent<TagComponent>("PlayerSpaceship");
    addComponent<DynamicPositionComponent>(pos);
    addComponent<PlayerSpaceshipComponent>();
    addComponent<RenderableRectComponent>(20.0f, 20.0f);
    std::cout << "Created playerspaceship at " << (int)enttHandle << std::endl;
}



/*
** Universe
*/

Universe::Universe(){}


void testClick(){
    std::cout << "Click clock!" << std::endl;
}

void loadTestScene(Scene* s){
    {
      TestEntity t = TestEntity(s);
      t.addComponent<TagComponent>("TestEntity mouse circle");
      DynamicPositionComponent &posComp = t.addComponent<DynamicPositionComponent>(0.0f, 0.0f);
      posComp.lockToMouse();
      ColliderCircleComponent& coll = t.addComponent<ColliderCircleComponent>(100.0f, &posComp);
      ColliderComponent& genericcoll = t.getComponent<ColliderComponent>();
      genericcoll.active = true;
    }
}


void Universe::init(){
    spaceScene.init();
    activeScene = &spaceScene;

    //Create StarSystem
    StarSystem system = StarSystem(&spaceScene, "Solar System 1", {400.0f, 400.0f});
    Planet p = system.addRandomPlanet();

    //Create player ship
    PlayerSpaceship ship = PlayerSpaceship(&spaceScene, {400.0f, 200.0f});

    //Lock the camera on the planet
    //spaceScene.getCamera().setTarget(p);

    //Load BiomeScene
    PlanetComponent& pc = p.getComponent<PlanetComponent>();
    PlanetBiome b = PlanetBiome(pc.biomes[0], &spaceScene);
    PlanetBiomeComponent& pbc = b.getComponent<PlanetBiomeComponent>();
    this->planetScene = pbc.getBiomeScene();
    this->switchScene(this->planetScene);
    this->planetScene->getCamera().zoom = 0.3f;
}




void Universe::update(const Uint8 *keyState) {
    activeScene->update(keyState);
}


Scene* Universe::getCurrentScene(){
    return (activeScene == nullptr) ? &spaceScene : activeScene;
}

void Universe::switchScene(Scene* s){
    std::cout << "Switching to scene " << (void*)s << std::endl;
    this->activeScene = s;
}


Scene* Universe::getSpaceScene(){
    return &spaceScene;
}

Scene* Universe::getBiomeScene(){
    return this->planetScene;
}
