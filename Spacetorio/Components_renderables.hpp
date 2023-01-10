#ifndef COMPONENTS_RENDERABLES_H_
#define COMPONENTS_RENDERABLES_H_


#include "Camera.hpp"
#include "Components_generic.hpp"
#include "Components_position.hpp"
#include "SDL.h"

#include "Renderer.hpp"
extern Renderer* global_renderer;

/*
**
** Renderable Components
**
*/

//Base Component
struct RenderableComponent : public GenericComponent{
    RenderableComponent() = default;
    RenderableComponent(RenderableComponent* impl) : GenericComponent(impl) {}

    virtual void render(const PositionComponent& posComp, const Camera& cam) const{
        std::cout << "WARNING: Trying to render generic RenderableComponent! Use the impl instead." << std::endl;
    }
};

//Circle Renderer
struct RenderableCircleComponent : public RenderableComponent{
    float s = 0.0f;
    SDL_Color c = {255, 255, 255, 255};

    RenderableCircleComponent() = default;
    RenderableCircleComponent(const RenderableCircleComponent&) = default;
    RenderableCircleComponent(float s, SDL_Color c = {255,255,255,255}) : s(s), c(c) {}

    void render(const PositionComponent& posComp, const Camera& cam) const override{
        auto& pos = posComp.pos;
        const fPoint screenPos = cam.worldToScreen({pos.x, pos.y});
        global_renderer->drawCircle(screenPos.x, screenPos.y, this->s * cam.zoom, this->c);
    }
};

//Rect Renderer
struct RenderableRectComponent : public RenderableComponent{
    fSize size  = {0.0f, 0.0f};
    SDL_Color c = {255, 255, 255, 255};

    RenderableRectComponent() = default;
    RenderableRectComponent(const RenderableRectComponent&) = default;
    RenderableRectComponent(float w, float h, SDL_Color c = {255,255,255,255}) : size(w, h) {}

    void render(const PositionComponent& posComp, const Camera& cam) const override{
        auto& pos = posComp.pos;
        const fPoint screenPos = cam.worldToScreen({pos.x, pos.y});
        global_renderer->drawRect(screenPos.x, screenPos.y, size.w*cam.zoom, size.h*cam.zoom, this->c);
    }
};


#endif // COMPONENTS_RENDERABLES_H_
