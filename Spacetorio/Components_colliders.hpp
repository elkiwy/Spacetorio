#ifndef COMPONENTS_COLLIDERS_H_
#define COMPONENTS_COLLIDERS_H_


#include "Components_generic.hpp"
#include "Components_position.hpp"
#include "Components_renderables.hpp"

#include "Utils_geometry.hpp"


//Base Component
struct ColliderComponent : public GenericComponent{
    ColliderComponent() = default;
    ColliderComponent(GenericComponent* impl) : GenericComponent(impl) {}
    ShapeCircle dummyShape;

    bool active = false;
    SDL_Color debug_color = {255,128,0,255};

    bool checkCollisions(ColliderComponent* other){
        const Shape& s1 = this->getShape();
        const Shape& s2 = other->getShape();
        return s1.checkCollision(s2);
    }

    virtual const Shape& getShape(){
        std::cout << "WARNING: Called generic ColliderComponent.getShape() instead of impl one!" << std::endl;
        return dummyShape;
    }
};

//Circle Collider
struct ColliderCircleComponent : public ColliderComponent, public RenderableComponent{
    PositionComponent* posRef = nullptr;
    ShapeCircle shape;

    ColliderCircleComponent() = default;
    ColliderCircleComponent(const ColliderCircleComponent&) = default;
    ColliderCircleComponent(float radius, PositionComponent* posRef) : posRef(posRef), shape(ShapeCircle(posRef->pos, radius)) {}

    const Shape& getShape() override {
        shape.c = posRef->pos; return shape;
    }

    void render(const PositionComponent& posComp, const Camera& cam) const override{
        auto& pos = posComp.pos;
        const fPoint screenPos = cam.worldToScreen({pos.x, pos.y});
        global_renderer->drawCircle(screenPos.x, screenPos.y, shape.r * cam.zoom, debug_color);
    }
};


//Rectangle Collider
struct ColliderRectangleComponent : public ColliderComponent, public RenderableComponent{
    PositionComponent* posRef = nullptr;
    ShapeRectangle shape;
    SDL_Color col = {255,128,0,255};

    ColliderRectangleComponent() = default;
    ColliderRectangleComponent(const ColliderRectangleComponent&) = default;
    ColliderRectangleComponent(fSize size, PositionComponent* posRef) : posRef(posRef), shape(ShapeRectangle(posRef->pos, size)) {}

    inline fPoint getUpdatedPos(){return posRef->pos - (shape.size * 0.5f).toPoint();}
    const Shape& getShape() override{shape.pos = getUpdatedPos(); return shape;}
    const ShapeRectangle& getRectShape() {shape.pos = getUpdatedPos(); return shape;}

    void render(const PositionComponent& posComp, const Camera& cam) const override{
        auto& pos = posComp.pos;
        const fPoint screenPos = cam.worldToScreen({pos.x, pos.y});
        const fPoint center = {screenPos.x, screenPos.y};
        global_renderer->drawRect(center.x, center.y, shape.size.w*cam.zoom, shape.size.h*cam.zoom, debug_color);
    }
};


//Line Collider
struct ColliderLineComponent : public ColliderComponent, public RenderableComponent{
    PositionComponent* posRef = nullptr;
    ShapeLine baseShape;
    ShapeLine positionedShape;
    SDL_Color col = {255,128,0,255};

    ColliderLineComponent() = default;
    ColliderLineComponent(const ColliderLineComponent&) = default;
    ColliderLineComponent(fPoint p1, fPoint p2, PositionComponent* posRef) : posRef(posRef), baseShape(ShapeLine(p1, p2)), positionedShape(baseShape) {}

    const Shape& getShape() override{
        positionedShape.p1 = baseShape.p1 + posRef->pos;
        positionedShape.p2 = baseShape.p2 + posRef->pos;
        return positionedShape;
    }

    void render(const PositionComponent& posComp, const Camera& cam) const override{
        auto& pos = posComp.pos;
        const fPoint screenP1 = cam.worldToScreen(baseShape.p1 + pos);
        const fPoint screenP2 = cam.worldToScreen(baseShape.p2 + pos);
        global_renderer->drawLine(screenP1.x, screenP1.y, screenP2.x, screenP2.y, debug_color);
    }
};




#endif // COMPONENTS_COLLIDERS_H_
