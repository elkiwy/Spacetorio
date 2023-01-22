#ifndef SCENEBIOME_H_
#define SCENEBIOME_H_

#include "Scene.hpp"
#include "entt.hpp"
#include <array>
#include <vector>
#include "Utils_data.hpp"


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


struct TileBiome{
    std::vector<entt::entity> entities;
};

struct ChunkBiome{
    std::vector<std::vector<TileBiome>> tiles;
    std::vector<entt::entity> staticEntities;
    ShapeRectangle shape;
    int x = -1; int y = -1;
    ChunkBiome(int x, int y) :x(x), y(y),
         shape(fPoint(x*CHUNK_SIZE*TILE_SIZE,y*CHUNK_SIZE*TILE_SIZE),fSize(CHUNK_SIZE*TILE_SIZE, CHUNK_SIZE*TILE_SIZE)){}
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
};

#endif // SCENEBIOME_H_
