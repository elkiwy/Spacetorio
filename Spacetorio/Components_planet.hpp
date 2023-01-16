#ifndef COMPONENTS_PLANET_H_
#define COMPONENTS_PLANET_H_

#include "Components_renderables.hpp"
#include "Utils_geometry.hpp"
#include "Texture.hpp"
#include "SDL_stdinc.h"
#include <vector>

#define MAX_BIOMES 16

enum PlanetBiomeType { BIOME_FLATS, BIOME_MOUNTAIN, BIOME_LAKE };

struct PlanetComponent;

class PlanetBiome{
    public:
        PlanetBiome(PlanetBiomeType t):type(t){}
        ~PlanetBiome();

        void setData(fSize size, float curvature, float planetRadius, float direction);
        void generateTerrain();

        void render(fPoint planetPosOnScreen, float cameraZoom) const;


    private:
        PlanetBiomeType type = BIOME_FLATS;
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

        //TODO: const Uint8* chunkData_highRes = nullptr; //Used in biome-view
};




struct PlanetComponent : public RenderableComponent{
    float planetRadius = 0.0f;
    bool hasAtmosphere = false;
    entt::entity starSytem{entt::null};

    std::vector<PlanetBiome> biomes;

    PlanetComponent() = default;
    PlanetComponent(const PlanetComponent&) = default;
    PlanetComponent(float s, bool atm):planetRadius(s),hasAtmosphere(atm) {
        //Init test biomes
        biomes.emplace_back(BIOME_FLATS);
        biomes.emplace_back(BIOME_MOUNTAIN);
        biomes.emplace_back(BIOME_LAKE);
        biomes.emplace_back(BIOME_MOUNTAIN);
        biomes.emplace_back(BIOME_LAKE);
        biomes.emplace_back(BIOME_MOUNTAIN);

        //Init all the biomes
        const int nBiomes = biomes.size();
        const float angle = deg2rad(360.0f/(float)nBiomes);
        const float arcLength = planetRadius * angle;
        const float crustHeight = planetRadius*0.75f;
        for(int i=0;i<biomes.size();i++){
            float direction = rad2deg(0.0f + i*angle);
            biomes[i].setData(fSize(arcLength, crustHeight), angle, planetRadius, direction);
            biomes[i].generateTerrain();
        }
    }

    void render(const PositionComponent& posComp, const Camera& cam) const override{
        SDL_Color col = {255,255,255,255};
        auto& pos = posComp.pos;
        const float scaledRadius = planetRadius * cam.zoom;
        const fPoint planetScreenPos = cam.worldToScreen({pos.x, pos.y});
        //global_renderer->drawCircle(planetScreenPos.x, planetScreenPos.y, scaledRadius, col);

        //Render biomes guides
        const int nBiomes = biomes.size();
        const float biomesAngRad = deg2rad(360.0f/(float)nBiomes);
        const fPoint startPt = planetScreenPos + fPoint(scaledRadius, 0.0f);
        for (int i=0;i<nBiomes;i++){
            float currAng = biomesAngRad * i;
            fPoint p = startPt.rotatedPoint(currAng, planetScreenPos);
            global_renderer->drawLine(planetScreenPos.x, planetScreenPos.y, p.x, p.y, col);
        }

        //Render Biomes textures
        for(auto& biome : biomes){
            biome.render(planetScreenPos, cam.zoom);
        }

        //Re-render circle for guide
        //global_renderer->drawCircle(planetScreenPos.x, planetScreenPos.y, scaledRadius, {255,0,0,255});
    }
};


#endif // COMPONENTS_PLANET_H_
