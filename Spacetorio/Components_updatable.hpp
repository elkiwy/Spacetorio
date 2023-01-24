#ifndef COMPONENTS_UPDATABLE_H_
#define COMPONENTS_UPDATABLE_H_

#include "SDL_stdinc.h"
#include "SDL_keycode.h"
#include <iostream>

#include "Components_generic.hpp"

class Entity;

//Base Component
struct UpdatableComponent : public GenericComponent{
    UpdatableComponent() = default;
    UpdatableComponent(GenericComponent* impl) : GenericComponent(impl) {}

    virtual void update(Entity& e, const Uint8* keyState){
        std::cout << "WARNING: Trying to update generic UpdatableComponent! Use the impl instead." << std::endl;
    }

    virtual void onKeyPressed(SDL_Keycode key){}
};



#endif // COMPONENTS_UPDATABLE_H_
