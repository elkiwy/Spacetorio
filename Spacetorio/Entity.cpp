#include "Entity.hpp"

#include "Scene.hpp"

Entity::Entity(entt::entity handle, Scene *scene)
: enttHandle(handle), scene(scene) {
    reg = &scene->registry;
}

entt::registry& Entity::getMyRegistry() {
    if (reg == nullptr){reg = &scene->registry;}
    return scene->registry;
}


TestEntity::TestEntity(Scene *s) {
    scene = s; enttHandle = s->getRegistry().create();
}
