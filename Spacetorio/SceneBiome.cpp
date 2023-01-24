#include "SceneBiome.hpp"
#include "Entity.hpp"
#include "SDL_stdinc.h"
#include <string>
#include <vector>


#include "Utils_time.hpp"
#include "Components_colliders.hpp"

#include "Entity_player.hpp"


/*
** Utils
*/

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



/*
** TileBiome
*/

void TileBiome::addEntity(entt::entity e){
    this->entities.emplace_back(e);
}



/*
** ChunkBiome
*/

ChunkBiome::ChunkBiome(int x, int y, entt::registry& reg)
  :x(x),y(y),shape(fPoint(x*CT,y*CT),fSize(CT, CT)){

    //Instanciate all the empty chunks
    for(int j=0; j<CHUNK_SIZE; ++j){
        auto& row = tiles.emplace_back();
        for(int i=0; i<CHUNK_SIZE; ++i){
            row.emplace_back();
        }
    }
}

void ChunkBiome::addEntity(Entity e){
    this->entities.emplace(e.enttHandle);

    auto& genericPosition = e.getComponent<PositionComponent>();
    auto& pos = static_cast<PositionComponent*>(genericPosition.impl)->pos;
    int tileX = (((int)pos.x) / TILE_SIZE) % CHUNK_SIZE;
    int tileY = (((int)pos.y) / TILE_SIZE) % CHUNK_SIZE;
    tiles[tileY][tileX].addEntity(e.enttHandle);
}




/*
** SceneBiome
*/

SceneBiome::SceneBiome(){

}

SceneBiome::~SceneBiome(){

}

void SceneBiome::spawnPlayerAt(fPoint pos){
    PlayerEntity player = PlayerEntity(this, pos);
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
                auto& posComp = e.addComponent<StaticPositionComponent>(i*TILE_SIZE+TILE_SIZE/2, j*TILE_SIZE+TILE_SIZE/2);
                e.addComponent<RenderableRectComponent>(TILE_SIZE, TILE_SIZE);
                e.addComponent<ColliderRectangleComponent>(fSize(TILE_SIZE, TILE_SIZE), &posComp);

                ck.addEntity(e);
            }
        }
    }

    //Move the camera to the center of the scene
    this->getCamera().moveTo((mapW*TILE_SIZE)/2.0f, (mapH*TILE_SIZE)/2.0f);
}

void SceneBiome::render(){
    auto& cam = getCamera();
    auto& reg = getRegistry();

    //Render Static Chunks
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

    //Render the other dynamic objects (don't care about chunking since are few)
    auto dynamicEnts = reg.view<DynamicPositionComponent, RenderableComponent>();
    for(auto e: dynamicEnts){
        auto& pos = dynamicEnts.get<DynamicPositionComponent>(e);
        auto& renderable = dynamicEnts.get<RenderableComponent>(e);
        for(auto impl: renderable.impls){
            if (impl == nullptr){break;}
            static_cast<RenderableComponent*>(impl)->render(pos, cam);
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



TileBiome& SceneBiome::getTileAtWorldPos(float worldX, float worldY, iVec offset){
    int tileX = (((int)worldX) / TILE_SIZE) + offset.x;
    int tileY = (((int)worldY) / TILE_SIZE) + offset.y;
    int chunkX = tileX / CHUNK_SIZE;
    int chunkY = tileY / CHUNK_SIZE;
    int tileInChunkX = tileX % CHUNK_SIZE;
    int tileInChunkY = tileY % CHUNK_SIZE;
    return chunks[chunkY][chunkX].tiles[tileInChunkY][tileInChunkX];
}

TileBiome& SceneBiome::getTileAtTilePos(int tX, int tY){
    int chunkX = tX / CHUNK_SIZE;
    int chunkY = tY / CHUNK_SIZE;
    int tileInChunkX = tX % CHUNK_SIZE;
    int tileInChunkY = tY % CHUNK_SIZE;
    return chunks[chunkY][chunkX].tiles[tileInChunkY][tileInChunkX];
}

std::vector<TileBiome*> SceneBiome::getTilesInRect(const ShapeRectangle& rect){
    std::vector<TileBiome*> tiles;

    int tileMinX = floor(rect.pos.x / TILE_SIZE);
    int tileMinY = floor(rect.pos.y / TILE_SIZE);
    int tileMaxX = floor((rect.pos.x + rect.size.w) / TILE_SIZE);
    int tileMaxY = floor((rect.pos.y + rect.size.h) / TILE_SIZE);

    for(int j=tileMinY; j<=tileMaxY; j++){
        for(int i=tileMinX; i<=tileMaxX; i++){
            int chunkX = i / CHUNK_SIZE;
            int chunkY = j / CHUNK_SIZE;
            tiles.push_back(&chunks[chunkY][chunkX].tiles[j % CHUNK_SIZE][i % CHUNK_SIZE]);
        }
    }

    return tiles;
}
