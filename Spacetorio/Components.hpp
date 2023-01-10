#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include "Entity.hpp"

#include "Components_updatable.hpp"
#include "Components_position.hpp"

#include "Renderer.hpp"
#include <iostream>
extern Renderer* global_renderer;



struct TagComponent{
    std::string tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag) : tag(tag) {}
};




struct PlayerSpaceshipComponent : public UpdatableComponent {
    PlayerSpaceshipComponent() = default;
    PlayerSpaceshipComponent(const PlayerSpaceshipComponent &) = default;

    void update(Entity& e, const Uint8 *ks) override {
        float inc = 0.1f;
        fVec acc = {0.0f, 0.0f};
        if (ks[SDL_SCANCODE_D]) acc.x += inc;
        if (ks[SDL_SCANCODE_A]) acc.x -= inc;
        if (ks[SDL_SCANCODE_S]) acc.y += inc;
        if (ks[SDL_SCANCODE_W]) acc.y -= inc;
        auto& pos = e.getComponent<PositionComponent>();
        pos.spd += acc;
    }
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







#endif // COMPONENTS_H_
