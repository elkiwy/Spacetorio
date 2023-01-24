#ifndef SCENEBIOME_H_
#define SCENEBIOME_H_

#include "Scene.hpp"
#include "entt.hpp"
#include <array>
#include <string>
#include <vector>
#include "Utils_data.hpp"

#include "Components_position.hpp"
#include "Components_renderables.hpp"

#define CT CHUNK_SIZE*TILE_SIZE

/*

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

    void addEntity(entt::entity e);
    inline bool isEmpty() const{return entities.size() == 0;}
};



struct ChunkBiome{
    std::vector<std::vector<TileBiome>> tiles;
    entt::storage<void> entities;
    ShapeRectangle shape;
    int x = -1; int y = -1;

    ChunkBiome(int x, int y, entt::registry& reg);
    void addEntity(Entity e);
};



class SceneBiome : public Scene {
    public:
        SceneBiome();
        virtual ~SceneBiome();

        void init(SDL_Surface* terrain);
        void render() override;

        void spawnPlayerAt(fPoint pos);

        TileBiome& getTileAtWorldPos(float worldX, float worldY, iVec offset = {0,0});
        TileBiome& getTileAtTilePos(int tX, int tY);
        ChunkBiome& getChunk(float worldX, float worldY);

        std::vector<TileBiome*> getTilesInRect(const ShapeRectangle& rect);
        std::vector<TileBiome*> getTilesInLine(const ShapeLine& line);

    private:
        std::vector<std::vector<ChunkBiome>> chunks;
};

#endif // SCENEBIOME_H_
