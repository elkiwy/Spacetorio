#ifndef COMPONENTS_POSITION_H_
#define COMPONENTS_POSITION_H_

#include "SDL_mouse.h"
#include "SDL_stdinc.h"
#include "Utils_points.hpp"

#include "Components_updatable.hpp"

class Entity;

struct PositionComponent : public UpdatableComponent{
    fPoint pos = {0.0f,0.0f};
    fVec spd = {0.0f,0.0f};
    bool lockedToMouse = false;

    PositionComponent(const fPoint& p) : pos(p) {}
    PositionComponent(float x, float y) : pos({x,y}) {}

    void update(Entity& e, const Uint8* keyState) override;

    void lockToMouse(){lockedToMouse = true;}
    void unlockFromMouse(){lockedToMouse = false;}
};



#endif // COMPONENTS_POSITION_H_
