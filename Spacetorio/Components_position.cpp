#include "Components_position.hpp"
#include "Entity.hpp"
#include "Scene.hpp"

void PositionComponent::update(Entity& e, const Uint8* keyState){
    pos += spd;
    if (lockedToMouse){
        iPoint mousePos = {0,0};
        SDL_GetMouseState(&mousePos.x, &mousePos.y);
        fPoint worldPos = e.scene->getCamera().screenToWorld(fPoint(mousePos.x, mousePos.y));
        pos = worldPos;
    }
}
