#include "SceneBiome.hpp"
#include "Entity.hpp"
#include "SDL_stdinc.h"
#include <string>
#include <vector>



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

    //Instanciate all the empty chunks
    entt::registry& reg = getRegistry();
    for(int j=0; j<chunkCountH; ++j){
        auto& v = chunks.emplace_back();
        for(int i=0; i<chunkCountW; ++i){
            v.emplace_back(i, j, reg);
        }
    }

    std::cout << "Chunks: " << chunks.size() << " " << chunks[0].size() << std::endl;

    //Load all the entities from the terrainMap
    Uint8* terrainMapData = (Uint8*)terrainMap->pixels;
    for(int j=0; j<mapH; ++j){
        for(int i=0; i<mapW; ++i){
            int chunkX = i/CHUNK_SIZE;
            int chunkY = j/CHUNK_SIZE;
            Uint8 tileVal = terrainMapData[(i+(mapW*j))*4+0];
            if (tileVal > 250){
                //Create the entity
                Entity e = {this->newEntity(), this};
                //e.addComponent<StaticPositionComponent>(i*TILE_SIZE+TILE_SIZE/2, j*TILE_SIZE+TILE_SIZE/2);
                //e.addComponent<RenderableRectComponent>(TILE_SIZE, TILE_SIZE);

                ChunkBiome& ck = chunks[chunkY][chunkX];
                e.addComponentToPool<StaticPositionComponent>(ck.pool_positions.id, i*TILE_SIZE+TILE_SIZE/2, j*TILE_SIZE+TILE_SIZE/2);
                auto& comp = e.addComponent<RenderableRectComponent>(TILE_SIZE, TILE_SIZE);

                //TODO optimize this, I made it only for proof of concept, duplicate renderable components in two pools for each entity
                GenericComponent* impl = static_cast<GenericComponent*>(&comp);
                e.addComponentToPool<RenderableComponent>(ck.pool_renderables.id, impl);


                //Add it to the staticEntities in the correct chunk for faster queries for rendering
                ck.staticEntities.push_back(e.enttHandle);
            }
        }
    }

    //Move the camera to the center of the scene
    this->getCamera().moveTo((mapW*TILE_SIZE)/2.0f, (mapH*TILE_SIZE)/2.0f);
}


void SceneBiome::render(){
    auto& cam = getCamera();
    auto& camShape = cam.getCameraShape();
    auto& reg = getRegistry();
    fPoint cameraWorldPos = cam.getCameraWorldCenter();
    ChunkBiome& chunkOnCameraCenter = getChunk(cameraWorldPos.x, cameraWorldPos.y);

    //Scan the row that has the chunkOnCameraCenter and mark min and max chunk Xs
    int minChunkX = 999999; int maxChunkX = -1;
    for(int x=0;x<chunks[0].size();++x){
        ChunkBiome& c = chunks[chunkOnCameraCenter.y][x];
        if (c.shape.checkCollisionWithRectangle(camShape)){
            if (x<minChunkX) { minChunkX = x; }
            if (x>maxChunkX) { maxChunkX = x; }
        }
    }

    //Scan the col that has the chunkOnCameraCenter and mark min and max chunk Ys
    int minChunkY = 999999; int maxChunkY = -1;
    for(int y=0;y<chunks.size();++y){
        ChunkBiome& c = chunks[y][chunkOnCameraCenter.x];
        if (c.shape.checkCollisionWithRectangle(camShape)){
            if (y<minChunkY) { minChunkY = y; }
            if (y>maxChunkY) { maxChunkY = y; }
        }
    }


    //auto group = reg.group<StaticPositionComponent, RenderableComponent>();
    for(int j=minChunkY;j<=maxChunkY;++j){
        for(int i=minChunkX;i<=maxChunkX;++i){
            auto& chunkView = chunks[j][i].staticEntities;
            auto& chunk = chunks[j][i];

            for(auto& entity: chunkView){
                StaticPositionComponent& pos = chunk.pool_positions.pool->get(entity);
                RenderableComponent& renderable = chunk.pool_renderables.pool->get(entity);

                for(auto impl: renderable.impls){
                    if (impl == nullptr){break;}
                    static_cast<RenderableComponent*>(impl)->render(pos, cam);
                }
            }
        }
    }




    renderCameraCrosshair();
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
