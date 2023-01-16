#ifndef ENTITY_H_
#define ENTITY_H_

#include "entt.hpp"
#include <iostream>

class Scene;

class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);
        Entity(const Entity& other) = default;
        virtual ~Entity(){}

        template<typename T>
        bool hasComponent(){
            return getMyRegistry().any_of<T>(enttHandle);
        }

        template<typename T, typename... Args>
        T& addComponent(Args&&... args){
            assert(!hasComponent<T>() && "ERROR: Trying to add the same component to the same entity.");
            return getMyRegistry().emplace<T>(enttHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& getComponent(){
            assert(hasComponent<T>() && "ERROR: Entity doesn't have component to get.");
            return getMyRegistry().get<T>(enttHandle);
        }

        template<typename T>
        void removeComponent(){
            assert(hasComponent<T>() && "ERROR: Trying to remove a component missing from entity.");
            return getMyRegistry().remove<T>(enttHandle);
        }

        entt::entity enttHandle { 0 };
        entt::registry* reg = nullptr;
        Scene* scene = nullptr;

        entt::registry& getMyRegistry();
};



class TestEntity : public Entity{
    public:
        TestEntity() {}
        TestEntity(const TestEntity &p) = default;
        TestEntity(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        TestEntity(Scene *s);
        virtual ~TestEntity() {}

      private:
};




#endif // ENTITY_H_
