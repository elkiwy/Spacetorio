#ifndef COMPONENTS_POSITION_H_
#define COMPONENTS_POSITION_H_

#include "SDL_stdinc.h"
#include "Utils_geometry.hpp"

#include "Components_updatable.hpp"

class Entity;

struct PositionComponent : public UpdatableComponent{
    fPoint pos = {0.0f,0.0f};
    fVec spd = {0.0f,0.0f};

    PositionComponent(const fPoint& p) : pos(p) {}
    PositionComponent(float x, float y) : pos({x,y}) {}

    void update(Entity& e, const Uint8* keyState) override{
        pos += spd;
    }
};



#endif // COMPONENTS_POSITION_H_
