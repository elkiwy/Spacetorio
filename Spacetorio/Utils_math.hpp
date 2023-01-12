#ifndef UTILS_MATH_H_
#define UTILS_MATH_H_

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <stdlib.h>
#include <sys/_types/_size_t.h>
#include "PerlinNoise.hpp"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "Utils_geometry.hpp"

inline int randInt(int min, int max){
    return rand() % (max-min+1) + min;
}

inline float randFloat(){
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

inline float randFloat(float min, float max){
    return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}

inline SDL_Surface* createSurfaceFromNoise(fSize size, fVec scale, int octaves, float persistance, long seed){
    int imgW = size.w; int imgH = size.h;
    Uint8 pixelsData[imgW*imgH*4];
    const siv::PerlinNoise perlin{(siv::PerlinNoise::seed_type)seed};
    for (int j = 0; j < imgH; j+=1) {
        for (int i = 0; i < imgW; i+=1) {
            const double noise_01 = perlin.octave2D_01((i * scale.x), (j * scale.y), octaves, persistance);
            const double verticalGradient_01 = ((double)j) / (double)imgH;

            //const double final_01 = (noise_01 + verticalGradient_01)/2.0;
            const double final_01 = (noise_01 * verticalGradient_01);

            //Uint8 pixelVal = floor(final_01 * 255.0f);
            Uint8 pixelVal = (final_01 > 0.5) ? 255 : 0;
            pixelsData[(i+(imgH*j))*4 + 0] = pixelVal;
            pixelsData[(i+(imgH*j))*4 + 1] = pixelVal;
            pixelsData[(i+(imgH*j))*4 + 2] = pixelVal;
            pixelsData[(i+(imgH*j))*4 + 3] = (Uint8)255;
        }
    }
    return SDL_CreateRGBSurfaceFrom(pixelsData, imgW, imgH, 32, imgW*4, 0, 0, 0, 0);
}




#endif // UTILS_MATH_H_
