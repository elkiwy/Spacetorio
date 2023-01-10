#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include <array>
#include <iostream>
#include <string>
#include <vcruntime.h>
#include "SDL_pixels.h"
#include "SDL_stdinc.h"
#include "Utils_geometry.hpp"
#include "entt.hpp"

#include "Camera.hpp"

#include "Renderer.hpp"
extern Renderer* global_renderer;

struct PositionComponent;



struct TagComponent{
    std::string tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag) : tag(tag) {}
};


struct PositionComponent{
    fPoint pos = {0.0f,0.0f};
    fVec spd = {0.0f,0.0f};

    PositionComponent() = default;
    PositionComponent(const PositionComponent&) = default;
    PositionComponent(const fPoint& p) : pos(p) {}
    PositionComponent(float x, float y) : pos({x,y}) {}

    void update(const Uint8* ks){
        pos += spd;
    }
};


struct PlanetComponent{
    float planetSize = 0.0f;
    bool hasAtmosphere = false;
    entt::entity starSytem{entt::null};

    PlanetComponent() = default;
    PlanetComponent(const PlanetComponent&) = default;
    PlanetComponent(float s, bool atm) : planetSize(s), hasAtmosphere(atm) {}
};

struct StarSystemComponent{
    size_t nPlanets = 0;
    std::array<entt::entity, 10> planets = { entt::null };

    StarSystemComponent() = default;
    StarSystemComponent(const StarSystemComponent&) = default;

    void addPlanet(entt::entity e){
        for (int p = 0; p < 10; p++) {
            if (planets[p] == entt::null) {
                planets[p] = e;
            }
        }
    }
};




/*
**
** Renderable Components
**
*/

//Base Component
struct RenderableComponent{
    RenderableComponent* impl = nullptr;

    RenderableComponent() = default;
    RenderableComponent(const RenderableComponent&) = default;
    RenderableComponent(RenderableComponent* impl) : impl(impl) {}

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




#endif // COMPONENTS_H_
