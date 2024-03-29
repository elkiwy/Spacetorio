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
    RenderableComponent(GenericComponent* impl) : GenericComponent(impl) {}

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

//Tile Renderer
struct RenderableTileComponent{
    TileRenderData renderData;

    RenderableTileComponent() = default;
    RenderableTileComponent(const RenderableTileComponent&) = default;
    RenderableTileComponent(float ox, float oy) { renderData.textureAtlasOffset = {ox, oy};}

    const TileRenderData& getRenderInfo(const PositionComponent& posComp){
        renderData.pos.x = posComp.pos.x;
        renderData.pos.y = posComp.pos.y;
        return renderData;
    }
};

//Sprite Renderer
struct RenderableSpriteComponent{
    SpriteRenderData renderData;

    RenderableSpriteComponent() = default;
    RenderableSpriteComponent(const RenderableSpriteComponent&) = default;
    RenderableSpriteComponent(int sprInd, fSize spriteSize) {
        renderData.spriteIndex = sprInd;
        renderData.size.x = spriteSize.w;
        renderData.size.y = spriteSize.h;
    }

    const SpriteRenderData& getRenderInfo(const PositionComponent& posComp){
        renderData.pos.x = posComp.pos.x;
        renderData.pos.y = posComp.pos.y;
        return renderData;
    }
};



#endif // COMPONENTS_RENDERABLES_H_
