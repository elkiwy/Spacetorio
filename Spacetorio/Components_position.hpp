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
    fPoint spd = {0.0f,0.0f};

    PositionComponent() = default;
    PositionComponent(MonoGenericComponent* impl) : MonoGenericComponent(impl) {}
    PositionComponent(const fPoint& p) : pos(p){}
    PositionComponent(float x, float y) : pos(x,y){}
};







#endif // COMPONENTS_POSITION_H_
