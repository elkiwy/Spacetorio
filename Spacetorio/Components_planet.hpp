#ifndef COMPONENTS_PLANET_H_
#define COMPONENTS_PLANET_H_

#include "Components_renderables.hpp"
#include "SDL_stdinc.h"
#include <vector>

#define MAX_BIOMES 16

enum PlanetBiomeType { BIOME_FLATS, BIOME_MOUNTAIN, BIOME_LAKE };

struct PlanetComponent;

class PlanetBiome{
    public:
        PlanetBiome(PlanetBiomeType t, PlanetComponent* ref):type(t),planetRef(ref){}
        PlanetBiomeType getBiome(){return type;}

        void setSize(fSize size, float curvature){this->size = size; this->curvature = curvature;}
        void generateTerrain();

        void DEBUG_saveTerrainLowRes(std::string filename);

    private:
        PlanetBiomeType type = BIOME_FLATS;
        Uint32 seed = 0;
        fSize size = {0.0f, 0.0f}; //Length of the arc on the planet and height of the chunk
        float curvature = 0.0f;
        PlanetComponent* planetRef = nullptr;

        size_t chunkData_lowRes_sizeW = 0;
        size_t chunkData_lowRes_sizeH = 0;
        std::vector<Uint8> chunkData_lowRes; //Used in space-view

        //TODO: const Uint8* chunkData_highRes = nullptr; //Used in biome-view
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
        biomes.emplace_back(BIOME_FLATS, this);
        biomes.emplace_back(BIOME_MOUNTAIN, this);
        biomes.emplace_back(BIOME_LAKE, this);
        biomes.emplace_back(BIOME_MOUNTAIN, this);
        biomes.emplace_back(BIOME_LAKE, this);
        biomes.emplace_back(BIOME_MOUNTAIN, this);
        this->initBiomes();
    }

    void initBiomes(){
        //Init all the biomes dimensions
        const int nBiomes = biomes.size();
        const float curvature = deg2rad(360.0f/(float)nBiomes);
        const float arcLength = planetSize * curvature;
        const float crustHeight = planetSize*0.5f;
        for(int i=0;i<biomes.size();i++){
            biomes[i].setSize(fSize(arcLength, crustHeight), curvature);
        }

        //Generate terraings
        auto& b = biomes[0];
        b.generateTerrain();

        b.DEBUG_saveTerrainLowRes("test_biome.bmp");
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
