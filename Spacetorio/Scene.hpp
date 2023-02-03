#ifndef SCENE_H_
#define SCENE_H_

#include "Camera.hpp"
#include "SDL_stdinc.h"
#include "entt.hpp"
#include "SDL.h"
#include "Components_generic.hpp"

#include "imgui.h"

class Renderer;
extern Renderer* global_renderer;

class Entity;


// Attach a generic component, so it's easier to do generic queries for events
// like registry.view<RenderableComponent>()
template <typename B, typename D>
void attachGenericComponent(entt::registry &reg, entt::entity ent) {
    D& derivedComp = reg.get<D>(ent);
    B* derivedCasted = static_cast<B*>(&derivedComp);

    //std::cout << "Attaching " << typeid(B).name() << " after having a " << typeid(D).name() << " connected to entity n: " << (int)ent << std::endl;

    if(reg.any_of<B>(ent)){
        //Has already the base class, register the other derived component to it
        B& generic = reg.get<B>(ent);
        generic.addImpl(static_cast<GenericComponent*>(derivedCasted));
    }else{
        //Create a new Generic component
        B& generic = reg.emplace<B>(ent, static_cast<GenericComponent*>(derivedCasted));
        //std::cout << "Created generic at " << (void*)&generic << " for derived " << (void*)&derivedComp << std::endl;
    }
}


// Attach a generic component, so it's easier to do generic queries for events
// like registry.view<RenderableComponent>()
template <typename B, typename D>
void attachMonoGenericComponent(entt::registry &reg, entt::entity ent) {
    D& derivedComp = reg.get<D>(ent);
    B* derivedCasted = static_cast<B*>(&derivedComp);
    B& generic = reg.emplace<B>(ent, static_cast<MonoGenericComponent*>(derivedCasted));
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

        template<typename B, typename D>
        void registerMonoGenericComponent() {
            registry.on_construct<D>().template connect<&attachMonoGenericComponent<B, D>>();
        }

        virtual void init();

        virtual void render();
        virtual void renderGUI();
        void renderCameraCrosshair();

        void update(const Uint8* keyState, const Uint32 mouseState, const iPoint& mousePos);
        void onMouseWheel(float dy);
        void onMouseLeftClick();
        void onKeyPressed(SDL_Keycode key);

        virtual void _checkClickables(const Uint32 mouseState, const iPoint& mousePos);

        Camera& getCamera() { return cam; }
        const Camera& getCamera() const { return cam; }
        entt::registry& getRegistry(){ return registry; }
        const entt::registry& getRegistry() const{ return registry; }
        entt::entity newEntity(){ return registry.create(); }

    private:
        Camera cam;
        entt::registry registry;

    friend class Entity;
};





#endif // SCENE_H_
