#ifndef SCENE_H_
#define SCENE_H_

#include "Camera.hpp"
#include "entt.hpp"

class Entity;

class Scene {
    public:
        Scene();
        Scene(const Scene& other) = delete;
        virtual ~Scene();

        Entity createEntity(const std::string& name = "");


        void render() const;
        void renderGUI() const;

    private:
        Camera cam;
        entt::registry registry;

    friend class Entity;
};


#endif // SCENE_H_
