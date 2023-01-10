#ifndef UNIVERSE_H_
#define UNIVERSE_H_

#include "Entity.hpp"
#include "SDL_stdinc.h"
#include "Scene.hpp"
#include <array>

class Planet : public Entity{
    public:
        Planet() {}
        Planet(const Planet &p) = default;
        Planet(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        Planet(Scene *s, std::string name, fPoint pos, float size);
        virtual ~Planet() {}

    private:
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

        void update(const Uint8* keyState);

        Scene& getCurrentScene(){return spaceScene;}

    private:
        Scene spaceScene;
};




#endif // UNIVERSE_H_
