#ifndef PLANET_H_
#define PLANET_H_

#include <vector>
#include "Scene.hpp"
#include "Utils_geometry.hpp"

#include "Entity.hpp"
#include "Renderer.hpp"
extern Renderer* global_renderer;

class Planet : protected Entity{
    public:
        Planet() {}
        Planet(const Planet &p) = default;
        Planet(entt::entity handle, Scene *s) {
            enttHandle = handle;
            scene = s;
        }
        Planet(entt::registry &r, Scene *s, std::string name, fPoint pos, float size) {
            enttHandle = r.create();
            scene = s;
            addComponent<TagComponent>((name.empty()) ? "_UnnamedPlanet_" : name);
            addComponent<PositionComponent>(pos);
            addComponent<PlanetComponent>(size, true);
            addComponent<RenderableCircleComponent>(size);
        }
        virtual ~Planet() {}

    private:
};


#endif // PLANET_H_
