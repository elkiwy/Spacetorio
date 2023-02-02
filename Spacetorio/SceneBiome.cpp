#include "SceneBiome.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "SDL_stdinc.h"
#include <string>
#include <vector>


#include "Utils_time.hpp"
#include "Components_colliders.hpp"

#include "Entity_player.hpp"
#include "Entity_tile.hpp"


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

void SceneBiome::spawnPlayerAt(fPoint pos){
    std::cout << "Spawning player at " << pos << std::endl;
    PlayerEntity player = PlayerEntity(this, pos);
}

void SceneBiome::init(SDL_Surface* terrainMap){
    Scene::init();

    std::cout << "Initializing SceneBiome " << std::endl;

    int mapW = terrainMap->w;
    int mapH = terrainMap->h;
    int chunkCountW = mapW/CHUNK_SIZE;
    int chunkCountH = mapH/CHUNK_SIZE;
    this->numTilesX = mapW;
    this->numTilesY = mapH;

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
            Uint8 tileVal = terrainMapData[(i+(mapW*((mapH-1) - j)))*4+0];


            bool randomTest = randInt(0,100) < 60;


            if (tileVal > 250 || randomTest){
                //Create the entity
                float posx = i*TILE_SIZE+TILE_SIZE/2.0f;
                float posy = j*TILE_SIZE+TILE_SIZE/2.0f;
                TileEntity e = TileEntity(this, fPoint(posx, posy));

                //Add the entity to its chunk
                chunks[chunkY][chunkX].addEntity(e);
            }
        }
    }


    //Update the tiles sprites
    auto tilesView = reg.view<TileComponent>();
    for(auto& tile : tilesView){
        auto& tileComp = tilesView.get<TileComponent>(tile);
        tileComp.updateSprite();
    }


    //Move the camera to the center of the scene
    this->getCamera().moveTo((mapW*TILE_SIZE)/2.0f, (mapH*TILE_SIZE)/2.0f);
}

void SceneBiome::render(){

    this->_renderChunkedTiles();

    this->_renderOtherSprites();

    //Add camera crosshair on top of everything
    renderCameraCrosshair();



    iPoint mousePos = {0,0};
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    fPoint worldMouse = getCamera().screenToWorld(fPoint(mousePos.x, mousePos.y));
    global_renderer->addRectToRenderCentered(worldMouse, {TILE_SIZE*0.75, TILE_SIZE*0.75}, {0,255,0,255});

    iPoint tileMouse = {(int)floor(worldMouse.x/TILE_SIZE), (int)floor(worldMouse.y/TILE_SIZE)};
    global_renderer->addRectToRenderCentered(fPoint(tileMouse.x*TILE_SIZE+TILE_SIZE*0.5f, tileMouse.y*TILE_SIZE+TILE_SIZE*0.5f), {TILE_SIZE*0.5, TILE_SIZE*0.5}, {0,200,255,255});
}

void SceneBiome::_renderChunkedTiles(){
    auto& cam = getCamera();
    auto& reg = getRegistry();

    //Get the chunks I have to render this frame
    int minChunkX,minChunkY,maxChunkX,maxChunkY;
    getChunksInCamera(cam, this->chunks, &minChunkX, &minChunkY, &maxChunkX, &maxChunkY);
    std::string currentChunkHash = std::to_string(minChunkX) + "_" + std::to_string(minChunkY) + "_" + std::to_string(maxChunkX) + "_" + std::to_string(maxChunkY);
    if (currentChunkHash != this->chunkHash){
        //Prepare the vector to hold all the chunk data
        std::vector<TileRenderData> tilesData;

        //Update the renderableTilesVBO if the chunks changed from the frame before
        auto globalView = reg.view<StaticPositionComponent, RenderableTileComponent>();
        for(int j=minChunkY;j<=maxChunkY;++j){
            for(int i=minChunkX;i<=maxChunkX;++i){
                auto& chunk = chunks[j][i];
                auto chunkView = (entt::basic_view{chunks[j][i].entities} | globalView);
                for(auto e: chunkView){
                    auto& pos = chunkView.get<StaticPositionComponent>(e);
                    auto& renderableTile = chunkView.get<RenderableTileComponent>(e);
                    const TileRenderData& td = renderableTile.getRenderInfo(pos);
                    tilesData.emplace_back(td);
                }
            }
        }

        //Send the tilesData to the renderer that will update the VBO on the gpu
        global_renderer->updateRenderableTilesVBO(tilesData);

        //Update the chunk hash
        this->chunkHash = currentChunkHash;
    }
}

void SceneBiome::_renderOtherSprites(){
    auto& reg = getRegistry();

    //Render the other dynamic objects (don't care about chunking since are few)
    std::vector<SpriteRenderData> spritesData;
    auto dynamicEnts = reg.view<DynamicPositionComponent, RenderableSpriteComponent>();
    for(auto e: dynamicEnts){
        auto& pos = dynamicEnts.get<DynamicPositionComponent>(e);
        auto& renderableSprite = dynamicEnts.get<RenderableSpriteComponent>(e);
        const SpriteRenderData& sp = renderableSprite.getRenderInfo(pos);
        spritesData.emplace_back(sp);
    }
    global_renderer->updateRenderableSpritesVBO(spritesData);
}

void SceneBiome::renderGUI(){
    Scene::renderGUI();

    ImGui::Begin("SceneBiome");
    ImGui::Text("SceneBiome");
    ImGui::Text("RenderableChunks: %s", chunkHash.c_str());

    //Get Camera world position
    fPoint cameraWorldPos = getCamera().getCameraWorldCenter();
    int chunkX = std::clamp((int)(cameraWorldPos.x / TILE_SIZE) / CHUNK_SIZE, 0, (int)chunks[0].size() - 1);
    int chunkY = std::clamp((int)(cameraWorldPos.y / TILE_SIZE) / CHUNK_SIZE, 0, (int)chunks.size()-1);
    ImGui::Text("Focussed chunk: [ %d, %d ]", chunkX, chunkY);


    iPoint mousePos = {0,0};
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    fPoint worldMouse = getCamera().screenToWorld(fPoint(mousePos.x, mousePos.y));
    int mousechunkX = std::clamp((int)(worldMouse.x / TILE_SIZE) / CHUNK_SIZE, 0, (int)chunks[0].size() - 1);
    int mousechunkY = std::clamp((int)(worldMouse.y / TILE_SIZE) / CHUNK_SIZE, 0, (int)chunks.size()-1);
    ImGui::Text("Chunk under mouse: [ %d, %d ]", mousechunkX, mousechunkY);

    ImGui::Text("Map chunk Size: [ %d, %d ]", (int)chunks[0].size(), (int)chunks.size());



    ImGui::Text("Tile value at mouse: [ %d ]", getTileSurroundingValue(worldMouse.x/TILE_SIZE, worldMouse.y/TILE_SIZE));

    ImGui::End();
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


/*
    Ref: http://www.cr31.co.uk/stagecast/wang/blob.html

 +------+------+------+
 | 128  |  1   |  2   |
 +------+------+------+
 |  64  |      |  4   |
 +------+------+------+
 |  32  |  16  |  8   |
 +------+------+------+

*/

int SceneBiome::getTileSurroundingValue(int tx, int ty){
    int v = 0;
    if (                    ty < numTilesY-1 && getTileAtTilePos(  tx, ty+1).isEmpty() == false){ v +=   1; }
    if (tx < numTilesX-1 && ty < numTilesY-1 && getTileAtTilePos(tx+1, ty+1).isEmpty() == false){ v +=   2; }
    if (tx < numTilesX-1                     && getTileAtTilePos(tx+1,   ty).isEmpty() == false){ v +=   4; }
    if (tx < numTilesX-1 &&           ty > 0 && getTileAtTilePos(tx+1, ty-1).isEmpty() == false){ v +=   8; }
    if (                              ty > 0 && getTileAtTilePos(  tx, ty-1).isEmpty() == false){ v +=  16; }
    if (          tx > 0 &&           ty > 0 && getTileAtTilePos(tx-1, ty-1).isEmpty() == false){ v +=  32; }
    if (          tx > 0                     && getTileAtTilePos(tx-1,   ty).isEmpty() == false){ v +=  64; }
    if (          tx > 0 && ty < numTilesY-1 && getTileAtTilePos(tx-1, ty+1).isEmpty() == false){ v += 128; }
    return v;
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
