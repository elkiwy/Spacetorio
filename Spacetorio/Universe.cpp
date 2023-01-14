#include "Universe.hpp"

#include "Components.hpp"
#include "Components_position.hpp"
#include "Components_renderables.hpp"
#include "Components_planet.hpp"

#include "Utils_math.hpp"

/*
** Planet
*/

Planet::Planet(Scene *s, std::string name, fPoint pos, float size) {
    scene = s; enttHandle = s->getRegistry().create();
    addComponent<TagComponent>((name.empty()) ? "_UnnamedPlanet_" : name);
    addComponent<PositionComponent>(pos);
    addComponent<PlanetComponent>(size, true);
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
    float size = 500.0f;//randFloat(100.0f, 500.0f);
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

Universe::Universe(){
}

void Universe::init(){
    //Create StarSystem
    StarSystem system = StarSystem(&spaceScene, "Solar System 1", {400.0f, 400.0f});
    Planet p = system.addRandomPlanet();

    //Create player ship
    PlayerSpaceship ship = PlayerSpaceship(&spaceScene, {400.0f, 200.0f});

    //Lock the camera on the planet
    //spaceScene.getCamera().setTarget(p);
}

void Universe::update(const Uint8 *keyState) {
    spaceScene.update(keyState);
}
