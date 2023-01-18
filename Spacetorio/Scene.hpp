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

    std::cout << "Attaching " << typeid(B).name() << " after having a " << typeid(D).name() << " connected to entity n: " << (int)ent << std::endl;

    if(reg.any_of<B>(ent)){
        //Has already the base class, register the other derived component to it
        B& generic = reg.get<B>(ent);
        generic.addImpl(static_cast<GenericComponent*>(derivedCasted));
    }else{
        //Create a new Generic component
        B& generic = reg.emplace<B>(ent, static_cast<GenericComponent*>(derivedCasted));
        std::cout << "Created generic at " << (void*)&generic << " for derived " << (void*)&derivedComp << std::endl;
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
        void onMouseLeftClick();

        Camera& getCamera() { return cam; }
        entt::registry& getRegistry(){ return registry; }
        entt::entity newEntity(){ return registry.create(); }

    private:
        Camera cam;
        entt::registry registry;


        ImVec2 curveValues[10];
        int curveSelectionInd = -1;


    friend class Entity;
};





#endif // SCENE_H_
