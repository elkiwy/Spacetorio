#ifndef SCENE_H_
#define SCENE_H_

#include "Camera.hpp"
#include "SDL_stdinc.h"
#include "entt.hpp"
#include "SDL.h"

#include "Components.hpp"

class Renderer;
extern Renderer* global_renderer;

class Entity;


// Attach a generic RenderableComponent too, so it's easier to do generic registry.view<RenderableComponent>()
template <typename C>
void attachBaseRenderable(entt::registry &reg, entt::entity ent) {
    auto &derivedRenderComp = reg.get<C>(ent);
    reg.emplace<RenderableComponent>(ent, &derivedRenderComp);
}


class Scene {
    public:
        Scene();
        Scene(const Scene& other) = delete;
        virtual ~Scene();

        template<typename C>
        void registerRenderableComponent() {
            registry.on_construct<C>().template connect<&attachBaseRenderable<C>>();
        }

        void render();
        void renderGUI();

        void update(const Uint8* keyState);
        void onMouseWheel(float dy);

        const Camera& getCamera() const{ return cam; }
        entt::registry& getRegistry(){ return registry; }

    private:
        Camera cam;
        entt::registry registry;


    friend class Entity;
};





#endif // SCENE_H_
