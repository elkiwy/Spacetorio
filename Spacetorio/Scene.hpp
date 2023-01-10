#ifndef SCENE_H_
#define SCENE_H_

#include "Camera.hpp"
#include "SDL_stdinc.h"
#include "entt.hpp"
#include "SDL.h"
#include "Components_generic.hpp"


class Renderer;
extern Renderer* global_renderer;

class Entity;


// Attach a generic component, so it's easier to do generic queries for events
// like registry.view<RenderableComponent>()
template <typename B, typename D>
void attachGenericComponent(entt::registry &reg, entt::entity ent) {
    auto &derivedComp = reg.get<D>(ent);

    if(reg.any_of<B>(ent)){
        //Has already the base class, register the other derived component to it
        GenericComponent& generic = reg.get<B>(ent);
        generic.addImpl(&derivedComp);
    }else{
        //Create a new Generic component
        reg.emplace<B>(ent, &derivedComp);
    }
}


class Scene {
    public:
        Scene();
        Scene(const Scene& other) = delete;
        virtual ~Scene();

        template<typename B, typename D>
        void registerGenericComponent() {
            registry.on_construct<D>().template connect<&attachGenericComponent<B, D>>();
        }

        void render();
        void renderGUI();

        void update(const Uint8* keyState);
        void onMouseWheel(float dy);

        Camera& getCamera() { return cam; }
        entt::registry& getRegistry(){ return registry; }

    private:
        Camera cam;
        entt::registry registry;


    friend class Entity;
};





#endif // SCENE_H_
