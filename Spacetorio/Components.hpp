#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include <array>
#include <string>
#include <vcruntime.h>
#include "SDL_pixels.h"
#include "Utils_geometry.hpp"

#include "Renderer.hpp"
extern Renderer* global_renderer;

struct TagComponent{
    std::string tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag) : tag(tag) {}
};


struct PositionComponent{
    fPoint pos = {0,0};

    PositionComponent() = default;
    PositionComponent(const PositionComponent&) = default;
    PositionComponent(const fPoint& p) : pos(p) {}
    PositionComponent(float x, float y) : pos({x,y}) {}
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


struct RenderableCircleComponent{
    float s = 0.0f;
    SDL_Color c = {255, 255, 255, 255};

    RenderableCircleComponent() = default;
    RenderableCircleComponent(const RenderableCircleComponent&) = default;
    RenderableCircleComponent(float s, SDL_Color c = {255,255,255,255}) : s(s), c(c) {}

    void render(const PositionComponent& posComp, const Camera& cam) const {
        auto pos = posComp.pos;
        const fPoint screenPos = cam.worldToScreen({pos.x, pos.y});
        global_renderer->drawCircle(screenPos.x, screenPos.y, this->s * cam.zoom, this->c);
    }
};




#endif // COMPONENTS_H_
