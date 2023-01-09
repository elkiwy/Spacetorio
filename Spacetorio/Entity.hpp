#ifndef ENTITY_H_
#define ENTITY_H_

#include "Components.hpp"
#include "entt.hpp"
#include "Scene.hpp"

class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene) : enttHandle(handle), scene(scene){ }
        Entity(const Entity& other) = default;
        virtual ~Entity(){}

        template<typename T>
        bool hasComponent(){
            return scene->registry.any_of<T>(enttHandle);
        }

        template<typename T, typename... Args>
        T& addComponent(Args&&... args){
            assert(!hasComponent<T>() && "ERROR: Trying to add the same component to the same entity.");
            return scene->registry.emplace<T>(enttHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& getComponent(){
            assert(!hasComponent<T>() && "ERROR: Entity doesn't have component to get.");
            return scene->registry.get<T>(enttHandle);
        }

        template<typename T>
        void removeComponent(){
            assert(!hasComponent<T>() && "ERROR: Trying to remove a component missing from entity.");
            return scene->registry.remove<T>(enttHandle);
        }

        entt::entity enttHandle { 0 };
        Scene* scene = nullptr;
};


#endif // ENTITY_H_
