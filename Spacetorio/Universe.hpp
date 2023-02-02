#ifndef UNIVERSE_H_
#define UNIVERSE_H_

#include "Entity.hpp"
#include "SDL_stdinc.h"
#include "Scene.hpp"
#include <array>

#include "SceneBiome.hpp"
#include "Utils_data.hpp"


class PlanetBiome : public Entity{
    public:
        PlanetBiome() {}
        PlanetBiome(const PlanetBiome &p) = default;
        PlanetBiome(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        PlanetBiome(Entity planetEntity, PlanetBiomeType t);
        virtual ~PlanetBiome() {}
};



class Planet : public Entity{
    public:
        Planet() {}
        Planet(const Planet &p) = default;
        Planet(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        Planet(Scene *s, std::string name, fPoint pos, float size);
        virtual ~Planet() {}
};



class StarSystem : public Entity {
    public:
        StarSystem(){}
        StarSystem(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        StarSystem(Scene *s, std::string name, fPoint pos);
        virtual ~StarSystem(){}

        Planet addRandomStar() ;
        Planet addRandomPlanet() ;
};



class PlayerSpaceship : public Entity{
    public:
        PlayerSpaceship() {}
        PlayerSpaceship(const PlayerSpaceship &p) = default;
        PlayerSpaceship(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        PlayerSpaceship(Scene *s, fPoint pos);
        virtual ~PlayerSpaceship() {}
};



class Universe {
    public:
        Universe();
        virtual ~Universe(){}

        void init();
        void update(const Uint8* keyState, const Uint32 mouseState, const iPoint& mousePos);

        Scene* getCurrentScene();
        void switchScene(Scene* s);
        Scene* getSpaceScene();
        Scene* getBiomeScene();

    private:
        Scene spaceScene;
        Scene* activeScene = nullptr;
        Scene* planetScene = nullptr;
};




#endif // UNIVERSE_H_
