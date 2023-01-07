#include "Entity.hpp"
#include <utility>

Entity::Entity(entt::entity handle, Scene* scene) : enttHandle(handle), scene(scene){

}

Entity::~Entity(){

}


