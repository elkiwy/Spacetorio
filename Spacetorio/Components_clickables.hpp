#ifndef COMPONENTS_CLICKABLES_H_
#define COMPONENTS_CLICKABLES_H_

#include "Components_generic.hpp"
#include "Components_position.hpp"
#include "Components_renderables.hpp"

#include "Utils_geometry.hpp"


//Base Component
struct ClickableComponent : public GenericComponent{
    ClickableComponent() = default;
    ClickableComponent(GenericComponent* impl) : GenericComponent(impl) {}

    bool active = true;
    void (*onclick)() = nullptr;
    bool hovered = false;

    SDL_Color debug_color_normal = {128,64,0,255};
    SDL_Color debug_color_hovered = {255,128,0,255};

    ShapeCircle dummyShape;
    virtual const Shape& getShape(){
        std::cout << "WARNING: Called generic ClickableComponent.getShape() instead of impl one!" << std::endl;
        return dummyShape;
    }

    virtual bool checkHovered(const ShapePoint& mouseWorldPt){
        std::cout << "WARNING: Called generic ClickableComponent.getShape() instead of impl one!" << std::endl;
        return hovered;
    }

    void click(){
        this->onclick();
    }

};

//Circle Clickable
struct ClickableCircleComponent : public ClickableComponent, public RenderableComponent{
    PositionComponent* posRef = nullptr;
    ShapeCircle shape;

    ClickableCircleComponent() = default;
    ClickableCircleComponent(const ClickableCircleComponent&) = default;
    ClickableCircleComponent(float radius, PositionComponent* posRef) : posRef(posRef), shape(ShapeCircle(posRef->pos, radius)) {}

    const Shape& getShape() override {shape.c = posRef->pos; return shape;}

    void render(const PositionComponent& posComp, const Camera& cam) const override{
        auto& pos = posComp.pos;
        const fPoint screenPos = cam.worldToScreen({pos.x, pos.y});
        SDL_Color col = (hovered) ? debug_color_hovered : debug_color_normal;
        global_renderer->drawCircle(screenPos.x, screenPos.y, shape.r * cam.zoom, col);
    }

    bool checkHovered(const ShapePoint& mouseWorldPt) override{
        this->hovered = shape.checkCollisionWithPoint(mouseWorldPt);
        return this->hovered;
    }
};


//Rectangle Clickable
struct ClickableRectangleComponent : public ClickableComponent, public RenderableComponent{
    PositionComponent* posRef = nullptr;
    ShapeRectangle shape;
    SDL_Color col = {255,128,0,255};

    ClickableRectangleComponent() = default;
    ClickableRectangleComponent(const ClickableRectangleComponent&) = default;
    ClickableRectangleComponent(fSize size, PositionComponent* posRef) : posRef(posRef), shape(ShapeRectangle(posRef->pos, size)) {}

    const Shape& getShape() override{shape.pos = posRef->pos; return shape;}

    void render(const PositionComponent& posComp, const Camera& cam) const override{
        auto& pos = posComp.pos;
        const fPoint screenPos = cam.worldToScreen({pos.x, pos.y});
        const fPoint center = {screenPos.x + (cam.zoom*shape.size.w/2), screenPos.y + (cam.zoom*shape.size.h/2)};
        SDL_Color col = (hovered) ? debug_color_hovered : debug_color_normal;
        global_renderer->drawRect(center.x, center.y, shape.size.w*cam.zoom, shape.size.h*cam.zoom, col);
    }

    bool checkHovered(const ShapePoint& mouseWorldPt) override{
        this->hovered = shape.checkCollisionWithPoint(mouseWorldPt);
        return this->hovered;
    }
};





#endif // COMPONENTS_CLICKABLES_H_