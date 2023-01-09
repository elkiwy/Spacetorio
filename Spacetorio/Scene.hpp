#ifndef SCENE_H_
#define SCENE_H_

#include "Camera.hpp"
#include "SDL_stdinc.h"
#include "entt.hpp"
#include "SDL.h"

class Renderer;
extern Renderer* global_renderer;

class Entity;

class Scene {
    public:
        Scene();
        Scene(const Scene& other) = delete;
        virtual ~Scene();

        void update();

        void render() const;
        void renderGUI();

        void onKeyDown(const Uint8* keyState);
        void onMouseWheel(float dy);

        const Camera& getCamera() const{ return cam; }
        entt::registry& getRegistry(){ return registry; }

    private:
        Camera cam;
        entt::registry registry;

    friend class Entity;
};


#endif // SCENE_H_
