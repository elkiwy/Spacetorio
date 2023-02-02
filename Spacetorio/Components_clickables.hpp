#ifndef COMPONENTS_CLICKABLES_H_
#define COMPONENTS_CLICKABLES_H_

#include "Components_generic.hpp"
#include "Components_position.hpp"
#include "Components_renderables.hpp"

#include "Utils_geometry.hpp"
#include <functional>


//Base Component
struct ClickableComponent : public GenericComponent{
    ClickableComponent() = default;
    ClickableComponent(GenericComponent* impl) : GenericComponent(impl) {}

    bool active = true;
    std::function<void()> onclick = nullptr;
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
        if (onclick == nullptr){std::cout << "WARNING: Called click() on a nullptr!" << std::endl;
        }else{this->onclick();}
    }

};

//Circle Clickable
struct ClickableCircleComponent : public ClickableComponent, public RenderableComponent{
    PositionComponent* posRef = nullptr;
    fPoint offset = {0.0f, 0.0f};
    ShapeCircle shape;

    ClickableCircleComponent() = default;
    ClickableCircleComponent(const ClickableCircleComponent&) = default;
    ClickableCircleComponent(float radius, PositionComponent* posRef, fPoint offset = {0.0f, 0.0f})
        : posRef(posRef), shape(ShapeCircle(posRef->pos, radius)), offset(offset) {}

    const Shape& getShape() override {shape.c = posRef->pos+offset; return shape;}

    void render(const PositionComponent& posComp, const Camera& cam) const override{
        auto& pos = posComp.pos;
        const fPoint screenPos = cam.worldToScreen({pos.x+offset.x, pos.y+offset.y});
        SDL_Color col = (hovered) ? debug_color_hovered : debug_color_normal;
        global_renderer->drawCircle(screenPos.x, screenPos.y, shape.r * cam.zoom, col);
    }

    bool checkHovered(const ShapePoint& mouseWorldPt) override{
        this->hovered = getShape().checkCollisionWithPoint(mouseWorldPt);
        return this->hovered;
    }
};


//Rectangle Clickable
struct ClickableRectangleComponent : public ClickableComponent, public RenderableComponent{
    PositionComponent* posRef = nullptr;
    fPoint offset = {0.0f, 0.0f};
    ShapeRectangle shape;
    SDL_Color col = {255,128,0,255};

    ClickableRectangleComponent() = default;
    ClickableRectangleComponent(const ClickableRectangleComponent&) = default;
    ClickableRectangleComponent(fSize size, PositionComponent* posRef, fPoint offset = {0.0f, 0.0f})
        : posRef(posRef), shape(ShapeRectangle(posRef->pos, size)), offset(offset) {}

    const Shape& getShape() override{
        shape.pos.x = posRef->pos.x + offset.x - (shape.size.w/2.0f);
        shape.pos.y = posRef->pos.y + offset.y - (shape.size.h/2.0f);
        return shape;
    }

    void render(const PositionComponent& posComp, const Camera& cam) const override{
        auto& pos = posComp.pos;
        global_renderer->addRectToRenderCentered(pos, fSize(shape.size.w, shape.size.h));
    }

    bool checkHovered(const ShapePoint& mouseWorldPt) override{
        this->hovered = getShape().checkCollisionWithPoint(mouseWorldPt);
        return this->hovered;
    }
};





#endif // COMPONENTS_CLICKABLES_H_
