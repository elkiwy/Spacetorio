#ifndef SCENEBIOME_H_
#define SCENEBIOME_H_

#include "Scene.hpp"
#include "entt.hpp"
#include <array>
#include <vector>

#define CHUNK_SIZE 8 //Tiles
#define GRID_SIZE CHUNK_SIZE*8 //Tiles
#define TILE_SIZE 32 //WorldCoordinates


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
    std::array<std::array<TileBiome, GRID_SIZE>, GRID_SIZE> tiles;
};

class SceneBiome : public Scene {
    public:
        SceneBiome();
        virtual ~SceneBiome();

        TileBiome& getTile(float worldX, float worldY);
        ChunkBiome& getChunk(float worldX, float worldY);
        void addEntityToTileAt(entt::entity e, float worldX, float worldY);

    private:
        std::array<std::array<ChunkBiome, GRID_SIZE>, GRID_SIZE> chunks;
};

#endif // SCENEBIOME_H_
