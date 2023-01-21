#ifndef SCENEBIOME_H_
#define SCENEBIOME_H_

#include "Scene.hpp"
#include "entt.hpp"
#include <array>
#include <vector>


/*

World
 0,0
   |
   V
   /-----+-----+-----+-----\
   | 0,0 | 1,0 | 2,0 | 3,0 |
   |-----+-----+-----+-----|
   | 0,1 | 1,1 | 2,1 | 3,1 |
   |-----+-----+-----+-----|
   | 0,2 | 1,2 | 2,2 | 3,2 |
   |-----+-----+-----+-----|
   | 0,3 | 1,3 | 2,3 | 3,3 |
   \-----+-----+-----+-----/



*/


struct TileBiome{
    std::vector<entt::entity> entities;
};

struct ChunkBiome{
    std::vector<std::vector<TileBiome>> tiles;
};

class SceneBiome : public Scene {
    public:
        SceneBiome();
        virtual ~SceneBiome();

        void init(SDL_Surface* terrain);

        TileBiome& getTileAtWorldPos(float worldX, float worldY);
        TileBiome& getTileAtTilePos(int tX, int tY);
        ChunkBiome& getChunk(float worldX, float worldY);
        void addEntityToTileAt(entt::entity e, float worldX, float worldY);

    private:
        std::vector<std::vector<ChunkBiome>> chunks;
};

#endif // SCENEBIOME_H_
