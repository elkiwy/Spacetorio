#include "SceneBiome.hpp"
#include "Components_position.hpp"
#include "Components_renderables.hpp"
#include "Entity.hpp"


SceneBiome::SceneBiome(iSize size){
    for(int j=GRID_SIZE/2; j<GRID_SIZE; j++){
        for(int i=0; i<GRID_SIZE; i++){
            Entity e = {this->newEntity(), this};
            e.addComponent<PositionComponent>(i*TILE_SIZE+TILE_SIZE/2, j*TILE_SIZE+TILE_SIZE/2);
            e.addComponent<RenderableRectComponent>(TILE_SIZE, TILE_SIZE);
        }
    }
}

SceneBiome::~SceneBiome(){

}



ChunkBiome& SceneBiome::getChunk(float worldX, float worldY){
    int tileX = worldX / TILE_SIZE;
    int tileY = worldY / TILE_SIZE;
    int chunkX = tileX / CHUNK_SIZE;
    int chunkY = tileY / CHUNK_SIZE;
    return chunks[chunkY][chunkX];
}

TileBiome& SceneBiome::getTile(float worldX, float worldY){
    int tileX = worldX / TILE_SIZE;
    int tileY = worldY / TILE_SIZE;
    int chunkX = tileX / CHUNK_SIZE;
    int chunkY = tileY / CHUNK_SIZE;
    int tileInChunkX = tileX % CHUNK_SIZE;
    int tileInChunkY = tileX % CHUNK_SIZE;
    return chunks[chunkY][chunkX].tiles[tileInChunkY][tileInChunkY];
}

void SceneBiome::addEntityToTileAt(entt::entity e, float worldX, float worldY){
    TileBiome& tile = getTile(worldX, worldY);
    tile.entities.push_back(e);
}
