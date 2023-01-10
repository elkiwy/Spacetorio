#ifndef UNIVERSE_H_
#define UNIVERSE_H_

#include <string>
#include <vector>
#include "Entity.hpp"
#include "SDL_stdinc.h"
#include "Scene.hpp"
#include "Utils_geometry.hpp"
#include "Utils_math.hpp"



class Planet : protected Entity{
    public:
        Planet() {}
        Planet(const Planet &p) = default;
        Planet(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        Planet(Scene *s, std::string name, fPoint pos, float size) {
            scene = s; enttHandle = s->getRegistry().create();
            addComponent<TagComponent>((name.empty()) ? "_UnnamedPlanet_" : name);
            addComponent<PositionComponent>(pos);
            addComponent<PlanetComponent>(size, true);
            auto& c = addComponent<RenderableCircleComponent>(size);
            std::cout << "Added " << (void*)&c << " to planet" << std::endl;
        }
        virtual ~Planet() {}

    private:
};



class StarSystem : protected Entity {
    public:
        StarSystem(){}
        StarSystem(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        StarSystem(Scene *s, std::string name, fPoint pos) {
            scene = s; enttHandle = s->getRegistry().create();
            addComponent<TagComponent>((name.empty()) ? "_UnnamedStarSystem_" : name);
            addComponent<PositionComponent>(pos);
            addComponent<StarSystemComponent>();


            Planet(scene, "Star", {0.0f, 0.0f}, 300.0f);

        }
        virtual ~StarSystem(){}

        void addRandomPlanets(int n) {
            for (int i = 0; i < n; i++) {
              fPoint pos = {randFloat(400.0f, 2000.0f), 0.0f};
              float size = randFloat(10.0f, 50.0f);
              std::string name = "RandomPlanet " + std::to_string(randInt(1000, 9999));
              Planet(scene, name, pos, size);
            }
        }
};



class PlayerSpaceship : protected Entity{
    public:
        PlayerSpaceship() {}
        PlayerSpaceship(const PlayerSpaceship &p) = default;
        PlayerSpaceship(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        PlayerSpaceship(Scene *s, fPoint pos) {
            scene = s; enttHandle = s->getRegistry().create();
            addComponent<TagComponent>("PlayerSpaceship");
            addComponent<PositionComponent>(pos);
            addComponent<RenderableRectComponent>(20.0f, 20.0f);
        }
        virtual ~PlayerSpaceship() {}


        void update(const Uint8* ks){
            float inc = 0.1f;
            fVec acc = {0.0f, 0.0f};
            if (ks[SDL_SCANCODE_D]) acc.x += inc;
            if (ks[SDL_SCANCODE_A]) acc.x -= inc;
            if (ks[SDL_SCANCODE_S]) acc.y += inc;
            if (ks[SDL_SCANCODE_W]) acc.y -= inc;

            auto& pos = getComponent<PositionComponent>();
            pos.spd += acc;
        }

};



class Universe {
    public:
        Universe(){
            auto s = StarSystem(&spaceScene, "Solar System 1", {400.0f, 400.0f});
            s.addRandomPlanets(8);

            PlayerSpaceship(&spaceScene, {400.0f, 200.0f});
        }
        virtual ~Universe(){}

        void update(const Uint8* keyState){
            spaceScene.update(keyState);
        }

        Scene& getCurrentScene(){return spaceScene;}

    private:
        Scene spaceScene;
};




#endif // UNIVERSE_H_
