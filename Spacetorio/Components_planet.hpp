#ifndef COMPONENTS_PLANET_H_
#define COMPONENTS_PLANET_H_

#include "Components_renderables.hpp"
#include "Utils_geometry.hpp"
#include "Utils_data.hpp"
#include "Texture.hpp"
#include "SDL_stdinc.h"
#include <vector>

#define MAX_BIOMES 16


class SceneBiome;

struct PlanetComponent;

struct PlanetBiomeComponent{
    PlanetBiomeType type;
    Uint32 seed = 0;
    fSize size = {0.0f, 0.0f}; //Length of the arc on the planet and height of the chunk
    float curvature = 0.0f;
    float direction = 0.0f;
    float planetRadius = 0.0f;

    size_t chunkData_lowRes_sizeW = 0;
    size_t chunkData_lowRes_sizeH = 0;
    std::vector<Uint8> chunkData_lowRes; //Used in space-view

    SDL_Surface* surface_flat = nullptr;
    SDL_Surface* surface_space = nullptr;
    Texture texture_space;

    SceneBiome* biomeScene = nullptr;

    PlanetBiomeComponent() = default;
    PlanetBiomeComponent(const PlanetBiomeComponent&) = default;
    PlanetBiomeComponent(PlanetBiomeType t);
    ~PlanetBiomeComponent();

    void setData(fSize size, float curvature, float planetRadius, float direction);
    void generateTerrain();

    SceneBiome* getBiomeScene();

    void render(fPoint planetPosOnScreen, float cameraZoom) const;
};


struct PlanetComponent : public RenderableComponent{
    float planetRadius = 0.0f;
    Entity myEntity;
    std::vector<entt::entity> biomes;

    PlanetComponent() = default;
    PlanetComponent(const PlanetComponent&) = default;
    PlanetComponent(Entity planetEntity, float s);

    void addBiome(PlanetBiomeType t);

    void render(const PositionComponent& posComp, const Camera& cam) const override;
};


#endif // COMPONENTS_PLANET_H_
