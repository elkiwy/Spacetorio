#ifndef UTILS_DATA_H_
#define UTILS_DATA_H_


#include "glm/glm.hpp"

#include <cmath>
#define CHUNK_SIZE 16 //Tiles
#define TILE_SIZE 32 //WorldCoordinates


inline int roundTo(int n, int group){
    return floor(n/group)*group;
}

inline int roundChunk(int n){
    return floor(n/CHUNK_SIZE)*CHUNK_SIZE;
}


enum PlanetBiomeType { BIOME_FLATS, BIOME_MOUNTAIN, BIOME_LAKE };



struct TileRenderData{
    glm::vec2 pos = {0.0f,0.0f};
    int spriteIndex = 0;
};

struct SpriteRenderData{
    glm::vec2 pos = {0.0f,0.0f};
    int spriteIndex = 0;
    glm::vec2 size = {0.0f, 0.0f};
};

struct LineRenderData{
    glm::vec2 p1 = {0.0f,0.0f};
    glm::vec2 p2 = {0.0f,0.0f};
    glm::vec3 col = {0.0f,0.0f,0.0f};
};



#endif // UTILS_DATA_H_
