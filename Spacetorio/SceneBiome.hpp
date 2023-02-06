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
   | 0,3 | 1,3 | 2,3 | 3,3 |
   |-----+-----+-----+-----|
   | 0,2 | 1,2 | 2,2 | 3,2 |
   |-----+-----+-----+-----|
   | 0,1 | 1,1 | 2,1 | 3,1 |
   |-----+-----+-----+-----|
   | 0,0 | 1,0 | 2,0 | 3,0 |
   \-----+-----+-----+-----/

*/


struct TileBiome{
    std::vector<entt::entity> entities;

    void addEntity(entt::entity e);
    void removeEntity(entt::entity e);
    inline bool isEmpty() const{return entities.size() == 0;}
};



struct ChunkBiome{
    std::vector<std::vector<TileBiome>> tiles;
    entt::storage<void> entities;
    ShapeRectangle shape;
    int x = -1; int y = -1;

    ChunkBiome(int x, int y, entt::registry& reg);
    void addEntity(Entity e);
    void removeEntity(Entity e);
};



class SceneBiome : public Scene {
    public:
        SceneBiome() {};
        virtual ~SceneBiome() {};

        void init(SDL_Surface* terrain);
        void render() override;
        void renderGUI() override;

        void update(const Uint8* keyState, const Uint32 mouseState, const iPoint& mousePos) override;
        void onMouseLeftClick(iPoint mousePos) override;

        void addTile(entt::entity e);
        void removeTile(entt::entity e);
        void _updateTileSurroundings(int tX, int tY);

        void _checkClickables(const Uint32 mouseState, const iPoint& mousePos) override;
        void _updateTile(int tX, int tY);

        void spawnPlayerAt(fPoint pos);

        TileBiome& getTileUnderMouse();
        TileBiome& getTileAtWorldPos(float worldX, float worldY, iVec offset = {0,0});
        TileBiome& getTileAtTilePos(int tX, int tY);
        int getTileSurroundingValue(int tx, int ty);
        ChunkBiome& getChunk(float worldX, float worldY);

        std::vector<TileBiome*> getTilesInRect(const ShapeRectangle& rect);
        std::vector<TileBiome*> getTilesInLine(const ShapeLine& line);

    private:
        std::vector<std::vector<ChunkBiome>> chunks;
        std::string chunkHash = "";
        bool dirtyChunks = false;

        size_t numTilesX = 0;
        size_t numTilesY = 0;

    private:
        void _renderChunkedTiles();
        void _renderOtherSprites();
};

#endif // SCENEBIOME_H_
