#include "Components_planet.hpp"
#include <cassert>
#include <iostream>

void PlanetBiome::generateTerrain(){
    //Asset that I'm not doing stupid things
    assert((this->chunkData_lowRes_sizeW == 0 && this->chunkData_lowRes_sizeH == 0) && "ERROR: calling generateTerrain() on a biome with a terrain.");
    assert((this->size.w > 0.0f && this->size.h > 0.0f) && "ERROR: calling generateTerrain() on a biome without a size set first.");


    //Prepare the vector to hold the data
    const size_t chunkData_size = floor(this->size.w * this->size.h);
    const int pixelDataSize = 4; //RBGA
    chunkData_lowRes.reserve(chunkData_size*pixelDataSize);
    std::fill(chunkData_lowRes.begin(), chunkData_lowRes.end(), 0);

    std::cout << "Generating a terrain of size " << this->size.w << " " << this->size.h << std::endl;

    //Output handling
    int binaryThreshold = 128;
    bool binaryOutput = true;

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
    const siv::PerlinNoise perlin{(siv::PerlinNoise::seed_type)seed};
    for (int j = 0; j < imgH; j+=1) {
        for (int i = 0; i < imgW; i+=1) {
            //Vertical Gradient
            const float vgSize = ((float)imgH) * VG_Ratio;
            const float vgGap = (((float)imgH) - vgSize)/2.0f;
            float vgVal = (j-vgGap)/vgSize;
            float VG = (j<vgGap) ? 0.0f : ((j>vgSize+vgGap) ? 1.0f : vgVal);
            float VG_02 = VG * 2.0f;
            float VG_11 = (VG * 2.0f) - 1.0f;

            //Features - Noise 2D
            double FM_tmpScaleY = FM_ScaleY;
            if (FM_ScaleSame){FM_tmpScaleY = FM_ScaleX;};
            const double FM = perlin.octave2D_01((i * FM_ScaleX), (j * FM_tmpScaleY), FM_Octaves, FM_Persistance);

            //AltitudeMap - Noise 1D
            double AM = perlin.octave1D_01((i * AM_Scale), AM_Octaves);
            AM = AM + VG_11;
            AM = fmin(fmax(AM, 0.0001f), 0.9999f);

            //AltitudeMap2 - Noise 1D
            const double AM2 = perlin.octave1D_01((i * AM2_Scale), AM2_Octaves);

            //Group and finalize
            double tot = AM_Influence + FM_Influence + VG_Influence + AM2_Influence;
            double final_01 = (FM*FM_Influence + AM*AM_Influence + AM2*AM2_Influence + VG*VG_Influence)/tot;

            Uint8 pixelVal = floor(final_01 * 255.0f);
            if (binaryOutput){pixelVal = (pixelVal > binaryThreshold) ? 255 : 0;}
            chunkData_lowRes[(i+(imgW*j))*4 + 0] = pixelVal;
            chunkData_lowRes[(i+(imgW*j))*4 + 1] = pixelVal;
            chunkData_lowRes[(i+(imgW*j))*4 + 2] = pixelVal;
            chunkData_lowRes[(i+(imgW*j))*4 + 3] = (Uint8)255;
        }
    }

    this->chunkData_lowRes_sizeH = imgH;
    this->chunkData_lowRes_sizeH = imgW;
}




void PlanetBiome::DEBUG_saveTerrainLowRes(std::string filename){
    void* pixelsData = &chunkData_lowRes[0];
    int imgW = size.w; int imgH = size.h;
    SDL_Surface* s = SDL_CreateRGBSurfaceFrom(pixelsData, imgW, imgH, 32, imgW*4, 0, 0, 0, 0);
    SDL_SaveBMP(s, filename.c_str());
    SDL_FreeSurface(s);
}
