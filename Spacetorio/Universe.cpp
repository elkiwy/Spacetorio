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
    auto& posComp = planetEntity.getComponent<PositionComponent>();
    auto& myPosComp = addComponent<PositionComponent>(posComp.pos);
    addComponent<ClickableCircleComponent>(100.0f, &myPosComp);
    //addComponent<RenderableCircleComponent>(size);
}


/*
** Planet
*/

Planet::Planet(Scene *s, std::string name, fPoint pos, float size) {
    scene = s; enttHandle = s->getRegistry().create();
    addComponent<TagComponent>((name.empty()) ? "_UnnamedPlanet_" : name);
    addComponent<PositionComponent>(pos);
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
    addComponent<PositionComponent>(pos);
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
    float size = 300.0f;//randFloat(100.0f, 500.0f);
    std::string name = "RandomPlanet " + std::to_string(randInt(1000, 9999));
    return Planet(scene, name, pos, size);
}



/*
** PlayerSpaceship
*/

PlayerSpaceship::PlayerSpaceship(Scene *s, fPoint pos) {
    scene = s; enttHandle = s->getRegistry().create();
    addComponent<TagComponent>("PlayerSpaceship");
    addComponent<PositionComponent>(pos);
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
      PositionComponent &posComp = t.addComponent<PositionComponent>(0.0f, 0.0f);
      posComp.lockToMouse();
      ColliderCircleComponent& coll = t.addComponent<ColliderCircleComponent>(100.0f, &posComp);
      ColliderComponent& genericcoll = t.getComponent<ColliderComponent>();
      genericcoll.active = true;
    }

    {
      TestEntity t = TestEntity(s);
      t.addComponent<TagComponent>("TestEntity circle");
      PositionComponent &posComp = t.addComponent<PositionComponent>(0.0f, 0.0f);
      ColliderCircleComponent& coll = t.addComponent<ColliderCircleComponent>(100.0f, &posComp);
    }

    {
      TestEntity t = TestEntity(s);
      t.addComponent<TagComponent>("TestEntity rect");
      PositionComponent &posComp = t.addComponent<PositionComponent>(300.0f, 0.0f);
      ColliderRectangleComponent& coll = t.addComponent<ColliderRectangleComponent>(fSize(100.0f, 500.0f), &posComp);
    }

    {
      TestEntity t = TestEntity(s);
      t.addComponent<TagComponent>("TestEntity line");
      PositionComponent &posComp = t.addComponent<PositionComponent>(500.0f, 0.0f);
      ColliderLineComponent& coll = t.addComponent<ColliderLineComponent>(fPoint(0.0f, -100.0f), fPoint(50.0f, 100.0f), &posComp);
    }


    {
      TestEntity t = TestEntity(s);
      t.addComponent<TagComponent>("TestEntity clickable circle");
      PositionComponent &posComp = t.addComponent<PositionComponent>(0.0f, -300.0f);
      ClickableCircleComponent& clickable = t.addComponent<ClickableCircleComponent>(100.0f, &posComp);
      clickable.onclick = &testClick;
    }

    {
      TestEntity t = TestEntity(s);
      t.addComponent<TagComponent>("TestEntity clickable circle");
      PositionComponent &posComp = t.addComponent<PositionComponent>(300.0f, -300.0f);
      ClickableRectangleComponent& clickable = t.addComponent<ClickableRectangleComponent>(fSize(100.0f, 200.0f), &posComp);
      clickable.onclick = &testClick;

    }
}


void Universe::init(){
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
