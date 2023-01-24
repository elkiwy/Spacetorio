#ifndef COMPONENTS_POSITION_H_
#define COMPONENTS_POSITION_H_

#include "SDL_mouse.h"
#include "SDL_stdinc.h"
#include "Utils_points.hpp"

#include "Components_updatable.hpp"

class Entity;

//Base Component
struct PositionComponent : public MonoGenericComponent{
    fPoint pos = {0.0f,0.0f};

    PositionComponent() = default;
    PositionComponent(MonoGenericComponent* impl) : MonoGenericComponent(impl) {}
    PositionComponent(const fPoint& p) : pos(p){}
    PositionComponent(float x, float y) : pos(x,y){}
};

struct DynamicPositionComponent : public PositionComponent, public UpdatableComponent{
    fVec spd = {0.0f,0.0f};
    bool lockedToMouse = false;
    bool automaticUpdates = true;

    DynamicPositionComponent(const fPoint& p) : PositionComponent(p) {}
    DynamicPositionComponent(float x, float y) : PositionComponent(x,y) {}

    void update(Entity& e, const Uint8* keyState) override;

    void lockToMouse(){lockedToMouse = true;}
    void unlockFromMouse(){lockedToMouse = false;}
};


struct StaticPositionComponent : public PositionComponent{
    StaticPositionComponent(const fPoint& p) : PositionComponent(p) {}
    StaticPositionComponent(float x, float y) : PositionComponent(x,y) {}
};




#endif // COMPONENTS_POSITION_H_
