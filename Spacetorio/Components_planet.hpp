#ifndef COMPONENTS_PLANET_H_
#define COMPONENTS_PLANET_H_

#include "Components_renderables.hpp"
#include <vector>

#define MAX_BIOMES 16


enum PlanetBiomeType { BIOME_FLATS, BIOME_MOUNTAIN, BIOME_LAKE };

class PlanetBiome{
    public:
        PlanetBiome(PlanetBiomeType t):type(t){}
        PlanetBiomeType getBiome(){return type;}
    private:
        PlanetBiomeType type = BIOME_FLATS;
};



struct PlanetComponent : public RenderableComponent{
    float planetSize = 0.0f;
    bool hasAtmosphere = false;
    entt::entity starSytem{entt::null};

    std::vector<PlanetBiome> biomes;

    PlanetComponent() = default;
    PlanetComponent(const PlanetComponent&) = default;
    PlanetComponent(float s, bool atm):planetSize(s),hasAtmosphere(atm) {
        //Init test biomes
        biomes.emplace_back(BIOME_FLATS);
        biomes.emplace_back(BIOME_MOUNTAIN);
        biomes.emplace_back(BIOME_LAKE);
        biomes.emplace_back(BIOME_MOUNTAIN);
        biomes.emplace_back(BIOME_LAKE);
        biomes.emplace_back(BIOME_MOUNTAIN);
    }

    void render(const PositionComponent& posComp, const Camera& cam) const override{
        SDL_Color col = {255,255,255,255};
        auto& pos = posComp.pos;
        const float scaledSize = planetSize*cam.zoom;
        const fPoint screenPos = cam.worldToScreen({pos.x, pos.y});
        global_renderer->drawCircle(screenPos.x, screenPos.y, scaledSize, col);

        //Split in biomes
        const int nBiomes = biomes.size();
        const float biomesAngRad = deg2rad(360.0f/(float)nBiomes);
        const fPoint startPt = screenPos + fPoint(scaledSize, 0.0f);
        for (int i=0;i<nBiomes;i++){
            float currAng = biomesAngRad * i;
            fPoint p = startPt.rotatedPoint(currAng, screenPos);
            global_renderer->drawLine(screenPos.x, screenPos.y, p.x, p.y, col);
        }

    }
};


#endif // COMPONENTS_PLANET_H_
