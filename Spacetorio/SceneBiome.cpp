#include "SceneBiome.hpp"
#include "Components_position.hpp"
#include "Components_renderables.hpp"
#include "Entity.hpp"
#include "SDL_stdinc.h"
#include "Utils_data.hpp"

SceneBiome::SceneBiome(){

}

SceneBiome::~SceneBiome(){

}

void SceneBiome::init(SDL_Surface* terrainMap){
    Scene::init();

    std::cout << "Initializing SceneBiome " << std::endl;


    int mapW = terrainMap->w;
    int mapH = terrainMap->h;
    int chunkCountW = mapW/CHUNK_SIZE;
    int chunkCountH = mapH/CHUNK_SIZE;
    Uint8* terrainMapData = (Uint8*)terrainMap->pixels;
    for(int j=0; j<mapH; ++j){
        for(int i=0; i<mapW; ++i){
            int chunkX = i/CHUNK_SIZE;
            int chunkY = j/CHUNK_SIZE;

            Uint8 tileVal = terrainMapData[(i+(mapW*j))*4+0];
            if (tileVal > 250){
                Entity e = {this->newEntity(), this};
                e.addComponent<PositionComponent>(i*TILE_SIZE+TILE_SIZE/2, j*TILE_SIZE+TILE_SIZE/2);
                e.addComponent<RenderableRectComponent>(TILE_SIZE, TILE_SIZE);
            }
        }
    }

    //Move the camera to the center of the scene
    this->getCamera().moveTo((mapW*TILE_SIZE)/2.0f, (mapH*TILE_SIZE)/2.0f);
}


ChunkBiome& SceneBiome::getChunk(float worldX, float worldY){
    int tileX = worldX / TILE_SIZE;
    int tileY = worldY / TILE_SIZE;
    int chunkX = tileX / CHUNK_SIZE;
    int chunkY = tileY / CHUNK_SIZE;
    return chunks[chunkY][chunkX];
}

TileBiome& SceneBiome::getTileAtWorldPos(float worldX, float worldY){
    int tileX = worldX / TILE_SIZE;
    int tileY = worldY / TILE_SIZE;
    int chunkX = tileX / CHUNK_SIZE;
    int chunkY = tileY / CHUNK_SIZE;
    int tileInChunkX = tileX % CHUNK_SIZE;
    int tileInChunkY = tileX % CHUNK_SIZE;
    return chunks[chunkY][chunkX].tiles[tileInChunkY][tileInChunkY];
}

TileBiome& SceneBiome::getTileAtTilePos(int tX, int tY){
    int chunkX = tX / CHUNK_SIZE;
    int chunkY = tY / CHUNK_SIZE;
    int tileInChunkX = tX % CHUNK_SIZE;
    int tileInChunkY = tX % CHUNK_SIZE;
    return chunks[chunkY][chunkX].tiles[tileInChunkY][tileInChunkY];
}

void SceneBiome::addEntityToTileAt(entt::entity e, float worldX, float worldY){
    TileBiome& tile = getTileAtWorldPos(worldX, worldY);
    tile.entities.push_back(e);
}
