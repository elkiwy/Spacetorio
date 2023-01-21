#ifndef UTILS_DATA_H_
#define UTILS_DATA_H_



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



#endif // UTILS_DATA_H_
