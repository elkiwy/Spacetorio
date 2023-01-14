#ifndef UTILS_MATH_H_
#define UTILS_MATH_H_

#include <algorithm>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <vector>

#ifdef __APPLE__
#include <sys/_types/_size_t.h>
#endif

#include "PerlinNoise.hpp"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "Utils_points.hpp"
#include "imgui.h"

inline int randInt(int min, int max){
    return rand() % (max-min+1) + min;
}

inline float randFloat(){
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

inline float randFloat(float min, float max){
    return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}

class NoiseSurfaceGenerator{
  public:
    fSize size = {1000,500};
    long seed = time(NULL);
    int binaryThreshold = 128;
    bool binaryOutput = false;

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

    enum FeatureType {NOISE_1D, NOISE_2D, VERTGRAD};


    NoiseSurfaceGenerator(){}
    ~NoiseSurfaceGenerator(){}

    bool renderGUI(){
        bool upd = false;
        //Debug Texture
        ImGui::Begin("Debug Texture");
        ImGui::Text("Debug Texture");
        ImGui::Separator();


        upd = upd || ImGui::SliderFloat("SizeX",      &size.w,      100.0f, 1000.0f);
        upd = upd || ImGui::SliderFloat("SizeY",      &size.h,      100.0f, 1000.0f);

        ImGui::Text("AltitudeMap");
        upd = upd || ImGui::SliderFloat("Scale",   &AM_Scale, 0.001f, 0.030f);
        upd = upd || ImGui::SliderInt(  "Octaves", &AM_Octaves, 0, 10);
        ImGui::Separator();

        ImGui::Text("AltitudeMap2");
        upd = upd || ImGui::SliderFloat("Scale",   &AM2_Scale,   0.001f, 0.030f);
        upd = upd || ImGui::SliderInt(  "Octaves", &AM2_Octaves,      0,     10);
        ImGui::Separator();

        ImGui::Text("FeatureMap");
        upd = upd || ImGui::SliderFloat("ScaleX",      &FM_ScaleX,      0.001f, 0.100f);
        upd = upd || ImGui::SliderFloat("ScaleY",      &FM_ScaleY,      0.001f, 0.100f);
        upd = upd || ImGui::Checkbox(   "SameScale",   &FM_ScaleSame);
        upd = upd || ImGui::SliderInt(  "Octaves",     &FM_Octaves,          1,     10);
        upd = upd || ImGui::SliderFloat("Persistance", &FM_Persistance, 0.001f,   1.0f);
        ImGui::Separator();

        ImGui::Text("VerGradient");
        upd = upd || ImGui::SliderFloat("Ratio",     &VG_Ratio,     0.0f, 1.0f);
        ImGui::Separator();
        ImGui::Separator();

        ImGui::Text("Influences");
        upd = upd || ImGui::SliderFloat("AltitudeMap",  &AM_Influence,  0.0f, 6.0f);
        upd = upd || ImGui::SliderFloat("AltitudeMap2", &AM2_Influence, 0.0f, 6.0f);
        upd = upd || ImGui::SliderFloat("FeatureMap",   &FM_Influence,  0.0f, 6.0f);
        upd = upd || ImGui::SliderFloat("VertGrad",     &VG_Influence,  0.0f, 6.0f);
        ImGui::Separator();

        upd = upd || ImGui::SliderInt("Binary threshold", &binaryThreshold, 0, 255);
        upd = upd || ImGui::Checkbox("Binary output", &binaryOutput);
        upd = upd || ImGui::Button("Reload");

        if(ImGui::Button("Seed")){seed = time(NULL); upd = true;}

        ImGui::End();
        return upd;
    }


   /*

    0.0 -> 0.0 -> -1.0
    0.0 -> 0.0 -> -1.0
    0.2 -> 0.4 -> -0.8
    0.4 -> 0.8 -> -0.4
    0.5 -> 1.0 ->  0.0
    0.6 -> 1.2 ->  0.4
    0.8 -> 1.6 ->  0.8
    1.0 -> 2.0 ->  1.0
    1.0 -> 2.0 ->  1.0

    */


    SDL_Surface* createSurfaceFromNoise(){
        int imgW = floor(size.w); int imgH = floor(size.h);
        SDL_Surface* s = SDL_CreateRGBSurface(0, imgW, imgH, 32, 0, 0, 0, 0);
        Uint8* pixelsData = (Uint8*)s->pixels;
        std::cout << "using " << imgW << " " << imgH << std::endl;

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
                pixelsData[(i+(imgW*j))*4 + 0] = pixelVal;
                pixelsData[(i+(imgW*j))*4 + 1] = pixelVal;
                pixelsData[(i+(imgW*j))*4 + 2] = pixelVal;
                pixelsData[(i+(imgW*j))*4 + 3] = (Uint8)255;


                //pixelsData[(i+(imgW*j))*4 + 0] = 255;
                //pixelsData[(i+(imgW*j))*4 + 1] = 255;
                //pixelsData[(i+(imgW*j))*4 + 2] = 0;
                //pixelsData[(i+(imgW*j))*4 + 3] = 255;
            }
        }

        //void* pixelsData = &chunkData_lowRes[0];
        //int imgW = size.w; int imgH = size.h;
        //SDL_Surface* s = SDL_CreateRGBSurfaceFrom(pixelsData, imgW, imgH, 32, imgW*4, 0, 0, 0, 0);
        //SDL_SaveBMP(s, "testone.bmp");
        //SDL_FreeSurface(s);


        return s;
    }
};

























#endif // UTILS_MATH_H_
