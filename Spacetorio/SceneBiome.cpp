#include "SceneBiome.hpp"
#include "Entity.hpp"
#include "SDL_stdinc.h"
#include <string>
#include <vector>


#include "Utils_time.hpp"

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
                ChunkBiome& ck = chunks[chunkY][chunkX];
                e.addComponent<StaticPositionComponent>(i*TILE_SIZE+TILE_SIZE/2, j*TILE_SIZE+TILE_SIZE/2);
                e.addComponent<RenderableRectComponent>(TILE_SIZE, TILE_SIZE);
                ck.entities.emplace(e.enttHandle);
            }
        }
    }

    //Move the camera to the center of the scene
    this->getCamera().moveTo((mapW*TILE_SIZE)/2.0f, (mapH*TILE_SIZE)/2.0f);
}


void getChunksInCamera(Camera& cam, const std::vector<std::vector<ChunkBiome>>& chunks, int* minX, int* minY, int* maxX, int* maxY){
    //Get Camera world position
    auto& camShape = cam.getCameraShape();
    fPoint cameraWorldPos = cam.getCameraWorldCenter();

    //Get the main chunk in focus
    int chunkX = std::clamp((int)(cameraWorldPos.x / TILE_SIZE) / CHUNK_SIZE, 0, (int)chunks[0].size() - 1);
    int chunkY = std::clamp((int)(cameraWorldPos.y / TILE_SIZE) / CHUNK_SIZE, 0, (int)chunks.size()-1);

    //Scan the row that has the chunkOnCameraCenter and mark min and max chunk Xs
    *minX = 999999; *maxX = -1;
    for(int x=0;x<chunks[0].size();++x){
        const ChunkBiome& c = chunks[chunkY][x];
        if (c.shape.checkCollisionWithRectangle(camShape)){
            if (x<*minX) { *minX = x; }
            if (x>*maxX) { *maxX = x; }
        }
    }

    //Scan the col that has the chunkOnCameraCenter and mark min and max chunk Ys
    *minY = 999999; *maxY = -1;
    for(int y=0;y<chunks.size();++y){
        const ChunkBiome& c = chunks[y][chunkX];
        if (c.shape.checkCollisionWithRectangle(camShape)){
            if (y<*minY) { *minY = y; }
            if (y>*maxY) { *maxY = y; }
        }
    }
}

void SceneBiome::render(){
    auto& cam = getCamera();
    auto& reg = getRegistry();
    int minChunkX,minChunkY,maxChunkX,maxChunkY;
    getChunksInCamera(cam, this->chunks, &minChunkX, &minChunkY, &maxChunkX, &maxChunkY);

    auto globalView = reg.view<StaticPositionComponent, RenderableComponent>();
    for(int j=minChunkY;j<=maxChunkY;++j){
        for(int i=minChunkX;i<=maxChunkX;++i){
            auto& chunk = chunks[j][i];
            auto chunkView = (entt::basic_view{chunks[j][i].entities} | globalView);
            for(auto e: chunkView){
                auto& pos = chunkView.get<StaticPositionComponent>(e);
                auto& renderable = chunkView.get<RenderableComponent>(e);
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
