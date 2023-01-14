#include "Components_planet.hpp"
#include "SDL_pixels.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include <cassert>
#include <corecrt_math.h>
#include <iostream>
#include <vector>


SDL_Color getPixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + (y * surface->pitch) + x * bpp; /* Here p is the address to the pixel we want to retrieve */
    assert((bpp == 4) && "ERROR: using getPixel with a bpp different than 4! Look at https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface");
    Uint32 val = *(Uint32 *)p;
    SDL_Color rgb;
    SDL_GetRGB(val, surface->format, &rgb.r, &rgb.g, &rgb.b);
    return rgb;
}

iPoint dstToSrc(iPoint dst, int Ro, int Ri, iSize srcSize, float minAngle, float maxAngle){
    const float angle = atan2(dst.y-Ro, dst.x-Ro);
    if (angle > maxAngle || angle < minAngle){ return {-1, -1};}
    const float percAngle = (angle-minAngle) / (maxAngle-minAngle);
    const int distance = sqrt((dst.y-Ro)*(dst.y-Ro) + (dst.x-Ro)*(dst.x-Ro));
    const float percHeight = ((float)(distance-Ri)) / ((float)(Ro-Ri));
    const int targetSrcHeight = floor(percHeight * ((float)(srcSize.h)));
    const float percWidth = angle / (M_PI*2.0f);

    if (srcSize.h - targetSrcHeight - 1 == -1) {
        std::cout << "cazzo";
    }
    return {(int)(srcSize.w*percAngle) % srcSize.w, srcSize.h - targetSrcHeight - 1};
}

SDL_Surface* bendImageOnArc(SDL_Surface* src, float angle, int Ro, int Ri){
    const iSize dstSize = {Ro*2, Ro*2};
    SDL_Surface* dstSurface = SDL_CreateRGBSurface(0, dstSize.w, dstSize.h, 32, 0, 0, 0, 0);
    Uint8* dstPixels = (Uint8*)dstSurface->pixels;

    for (int i=0; i<Ro; i++){
        const int outer_radius = sqrt(Ro*Ro - i*i)-1;
        for(int j=-outer_radius; j<outer_radius; j++){
            const int inner_radius = (i<Ri) ? sqrt(Ri*Ri - i*i) : -1;
            if (j > inner_radius || j < -inner_radius){
                iPoint dstPoint = { Ro + j, Ro - i };
                iPoint srcPoint = dstToSrc(dstPoint, Ro, Ri, { src->w, src->h }, deg2rad(-(angle / 2.0f) - 90.0f), deg2rad((angle / 2.0f) - 90.0f));
                if (srcPoint.x > -1) {
                    SDL_Color col = getPixel(src, srcPoint.x, srcPoint.y);
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 0] = col.r;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 1] = col.g;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 2] = col.b;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 3] = 255;
                }

                dstPoint = { Ro + j, Ro + i };
                srcPoint = dstToSrc(dstPoint, Ro, Ri, { src->w, src->h }, deg2rad(-(angle / 2.0f) - 90.0f), deg2rad((angle / 2.0f) - 90.0f));
                if (srcPoint.x > -1) {
                    SDL_Color col = getPixel(src, srcPoint.x, srcPoint.y);
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 0] = col.r;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 1] = col.g;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 2] = col.b;
                    dstPixels[(dstPoint.x + (dstPoint.y * dstSize.w)) * 4 + 3] = 255;
                }


            }
        }
    }

    return dstSurface;
}







void PlanetBiome::generateTerrain() {
    //Asset that I'm not doing stupid things
    assert((this->chunkData_lowRes_sizeW == 0 && this->chunkData_lowRes_sizeH == 0) && "ERROR: calling generateTerrain() on a biome with a terrain.");
    assert((this->size.w > 0.0f && this->size.h > 0.0f) && "ERROR: calling generateTerrain() on a biome without a size set first.");


    //Prepare the vector to hold the data
    const size_t chunkData_size = floor(this->size.w) * floor(this->size.h);
    chunkData_lowRes = std::vector<Uint8>(chunkData_size * 4, 0);// 4 = RGBA
    //chunkData_lowRes.reserve(chunkData_size*pixelDataSize);
    //std::fill(chunkData_lowRes.begin(), chunkData_lowRes.end(), 0);

    std::cout << "Generating a terrain of size " << this->size.w << " " << this->size.h << std::endl;

    //Output handling
    int binaryThreshold = 128;
    bool binaryOutput = true;

    Uint32 seed = time(NULL);

    //Feature Map
    float FM_ScaleX = 0.020f;
    float FM_ScaleY = 0.020f;
    bool FM_ScaleSame = false;
    int FM_Octaves = 5;
    float FM_Persistance = 0.5f;
    float FM_Influence = 3.0f;

    //Altitude Map
    float AM_Scale = 0.01f;
    int AM_Octaves = 1;
    float AM_Influence = 3.0f;

    //Altitude Map 2
    float AM2_Scale = 0.03f;
    int AM2_Octaves = 3;
    float AM2_Influence = 2.0f;

    //Gradient
    float VG_Ratio = 0.45f;
    float VG_Influence = 4.0f;

    //Generate terrain
    int imgW = floor(size.w); int imgH = floor(size.h);
    const siv::PerlinNoise perlin{ (siv::PerlinNoise::seed_type)seed };
    for (int j = 0; j < imgH; j += 1) {
        for (int i = 0; i < imgW; i += 1) {
            //Vertical Gradient
            const float vgSize = ((float)imgH) * VG_Ratio;
            const float vgGap = (((float)imgH) - vgSize) / 2.0f;
            float vgVal = (j - vgGap) / vgSize;
            float VG = (j < vgGap) ? 0.0f : ((j > vgSize + vgGap) ? 1.0f : vgVal);
            float VG_02 = VG * 2.0f;
            float VG_11 = (VG * 2.0f) - 1.0f;

            //Features - Noise 2D
            double FM_tmpScaleY = FM_ScaleY;
            if (FM_ScaleSame) { FM_tmpScaleY = FM_ScaleX; };
            const double FM = perlin.octave2D_01((i * FM_ScaleX), (j * FM_tmpScaleY), FM_Octaves, FM_Persistance);

            //AltitudeMap - Noise 1D
            double AM = perlin.octave1D_01((i * AM_Scale), AM_Octaves);
            AM = AM + VG_11;
            AM = fmin(fmax(AM, 0.0001f), 0.9999f);

            //AltitudeMap2 - Noise 1D
            const double AM2 = perlin.octave1D_01((i * AM2_Scale), AM2_Octaves);

            //Group and finalize
            double tot = AM_Influence + FM_Influence + VG_Influence + AM2_Influence;
            double final_01 = (FM * FM_Influence + AM * AM_Influence + AM2 * AM2_Influence + VG * VG_Influence) / tot;

            Uint8 pixelVal = floor(final_01 * 255.0f);
            if (binaryOutput) { pixelVal = (pixelVal > binaryThreshold) ? 255 : 0; }
            chunkData_lowRes[(i + (imgW * j)) * 4 + 0] = pixelVal;
            chunkData_lowRes[(i + (imgW * j)) * 4 + 1] = pixelVal;
            chunkData_lowRes[(i + (imgW * j)) * 4 + 2] = pixelVal;
            chunkData_lowRes[(i + (imgW * j)) * 4 + 3] = (Uint8)255;
        }
    }

    this->chunkData_lowRes_sizeW = imgW;
    this->chunkData_lowRes_sizeH = imgH;

    std::cout << "Done generating terrain of size " << this->size.w << " " << this->size.h << std::endl;


    this->surface_flat = SDL_CreateRGBSurfaceFrom(&chunkData_lowRes[0], chunkData_lowRes_sizeW, chunkData_lowRes_sizeH, 32, chunkData_lowRes_sizeW * 4, 0, 0, 0, 0);
    const float Ro = this->planetRadius + this->size.h*0.5f;
    const float Ri = this->planetRadius - this->size.h*0.5f;
    this->surface_space = bendImageOnArc(this->surface_flat, rad2deg(this->curvature), Ro, Ri);
    this->texture_space = Texture(this->surface_space);
    //SDL_SaveBMP(surface_flat, "Terrain_normal.bmp");
    //SDL_SaveBMP(surface_space, "Terrain_bent.bmp");
}

PlanetBiome::~PlanetBiome(){
    if (surface_space != nullptr){SDL_FreeSurface(surface_space); surface_space = nullptr;}
    if (surface_flat != nullptr){SDL_FreeSurface(surface_flat); surface_flat = nullptr;}
}


void PlanetBiome::setData(fSize size, float curvature, float planetRadius, float direction){
    this->size = size; this->curvature = curvature;
    this->planetRadius = planetRadius;
    this->direction = direction;
}

void PlanetBiome::render(fPoint planetPosOnScreen, float cameraZoom) const{
    global_renderer->drawTexture(texture_space, planetPosOnScreen.x, planetPosOnScreen.y, direction, cameraZoom);
}
