#ifndef PLANET_H_
#define PLANET_H_

#include <vector>
#include "Scene.hpp"
#include "Utils_geometry.hpp"

#include "Renderer.hpp"
extern Renderer* global_renderer;

class Planet {
    public:
        Planet();
        Planet(const Planet& other) = default;
        virtual ~Planet();

        void render(const Camera& cam);

        const Scene& getCurrentScene();

    private:
        std::vector<Scene*> scenes = {}; //TODO handle this scenes in a controller object
        Scene* currentScene = nullptr;

        fPoint pos = {0.0, 0.0};
        float size = 0.0;
};


#endif // PLANET_H_
