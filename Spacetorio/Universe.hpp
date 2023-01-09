#ifndef UNIVERSE_H_
#define UNIVERSE_H_

#include <string>
#include <vector>
#include "Entity.hpp"
#include "Scene.hpp"
#include "Utils_geometry.hpp"
#include "Utils_math.hpp"



class Planet : protected Entity{
    public:
        Planet() {}
        Planet(const Planet &p) = default;
        Planet(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        Planet(Scene *s, std::string name, fPoint pos, float size) {
            scene = s;
            enttHandle = s->getRegistry().create();
            addComponent<TagComponent>((name.empty()) ? "_UnnamedPlanet_" : name);
            addComponent<PositionComponent>(pos);
            addComponent<PlanetComponent>(size, true);
            addComponent<RenderableCircleComponent>(size);
        }
        virtual ~Planet() {}

    private:
};


class StarSystem : protected Entity {
    public:
        StarSystem(){}
        StarSystem(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        StarSystem(Scene *s, std::string name, fPoint pos) {
            scene = s;
            enttHandle = s->getRegistry().create();
            addComponent<TagComponent>((name.empty()) ? "_UnnamedStarSystem_" : name);
            addComponent<PositionComponent>(pos);
            addComponent<StarSystemComponent>();
        }
        virtual ~StarSystem(){}

        void addRandomPlanets(int n) {
            for (int i = 0; i < n; i++) {
              fPoint pos = {randFloat(300.0f, 700.0f), randFloat(300.0f, 700.0f)};
              float size = randFloat(50.0f, 100.0f);
              std::string name = "RandomPlanet " + std::to_string(randInt(1000, 9999));
              Planet(scene, name, pos, size);
            }
        }

    private:
        std::vector<Planet*> planets = {};

};


class Universe {
    public:
        Universe(){
            auto s = StarSystem(&spaceScene, "Solar System 1", {400.0f, 400.0f});
            s.addRandomPlanets(8);
        }
        virtual ~Universe(){}

        Scene& getCurrentScene(){return spaceScene;}

    private:
        Scene spaceScene;
};

#endif // UNIVERSE_H_
