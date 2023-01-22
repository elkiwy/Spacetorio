#ifndef SCENEBIOME_H_
#define SCENEBIOME_H_

#include "Scene.hpp"
#include "entt.hpp"
#include <array>
#include <string>
#include <vector>
#include "Utils_data.hpp"
#include "Utils_chunkedStorage.hpp"

#include "Components_position.hpp"
#include "Components_renderables.hpp"

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


entt::dense_map<
    unsigned int,
    std::shared_ptr<
        entt::basic_sparse_set<
            entt::entity,
            std::allocator<entt::entity>
        >
    >,
    entt::identity,
    std::equal_to<unsigned int>,
    std::allocator<std::pair<
        const unsigned int,
        std::shared_ptr<
            entt::basic_sparse_set<
                entt::entity,
                std::allocator<entt::entity>
            >
        >
    >>
>


*/

#define CT CHUNK_SIZE*TILE_SIZE

struct TileBiome{
    std::vector<entt::entity> entities;
};

template<typename C>
struct DedicatedComponentPool{
    entt::storage_for_t<C>* pool = nullptr;
    std::string idStr = "";
    entt::hashed_string id;

    void init(std::string name, entt::registry& reg){
        this->idStr = name;
        this->id = entt::hashed_string(this->idStr.c_str());
        this->pool = &reg.storage<C>(this->id);
    }
};

struct ChunkBiome{
    std::vector<std::vector<TileBiome>> tiles;
    std::vector<entt::entity> staticEntities;


    DedicatedComponentPool<StaticPositionComponent> pool_positions;
    DedicatedComponentPool<RenderableComponent> pool_renderables;

    std::string idStr = "";

    ShapeRectangle shape;
    int x = -1; int y = -1;
    ChunkBiome(int x, int y, entt::registry& reg) : x(x), y(y), shape(fPoint(x*CT,y*CT),fSize(CT, CT)){
        this->idStr = std::to_string(x)+"_"+std::to_string(y);

        this->pool_positions.init("positions_"+this->idStr, reg);
        this->pool_renderables.init("renderables_"+this->idStr, reg);
    }

    void addRenderable(){

    }
};

class SceneBiome : public Scene {
    public:
        SceneBiome();
        virtual ~SceneBiome();

        void init(SDL_Surface* terrain);
        void render() override;

        TileBiome& getTileAtWorldPos(float worldX, float worldY);
        TileBiome& getTileAtTilePos(int tX, int tY);
        ChunkBiome& getChunk(float worldX, float worldY);
        void addEntityToTileAt(entt::entity e, float worldX, float worldY);

    private:
        std::vector<std::vector<ChunkBiome>> chunks;

        //entt::basic_registry<entt::entity, ChunkedStorage<entt::entity>> chunkedRegistry;
};

#endif // SCENEBIOME_H_
