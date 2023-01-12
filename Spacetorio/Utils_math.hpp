#ifndef UTILS_MATH_H_
#define UTILS_MATH_H_

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <stdlib.h>

#ifdef __APPLE__
#include <sys/_types/_size_t.h>
#endif

#include "PerlinNoise.hpp"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "Utils_geometry.hpp"
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
    fSize size = {500,500};
    fVec scale = {0.025f,0.01f};
    int octaves = 6;
    float persistance = 0.4f;
    long seed = time(NULL);
    int binaryThreshold = 128;
    bool binaryOutput = false;
    bool multiplyGradient = false;

    float vertGradientRatio = 0.15f;


    NoiseSurfaceGenerator(){}
    ~NoiseSurfaceGenerator(){}

    bool renderGUI(){
        bool needsUpdate = false;
        //Debug Texture
        ImGui::Begin("Debug Texture");
        ImGui::Text("Debug Texture");

        if (ImGui::SliderFloat("Scale X", &scale.x, 0.0f, 0.03f) ||
            ImGui::SliderFloat("Scale Y", &scale.y, 0.0f, 0.03f) ||
            ImGui::SliderInt("Binary threshold", &binaryThreshold, 0, 255) ||
            ImGui::SliderInt("Octaves", &octaves, 1, 10) ||
            ImGui::SliderFloat("Persistance", &persistance, 0.0f, 1.0f) ||
            ImGui::SliderFloat("VertGradRatio", &vertGradientRatio, 0.0f, 1.0f) ||
            ImGui::Checkbox("Binary output", &binaryOutput) ||
            ImGui::Checkbox("Gradient multiplication", &multiplyGradient) ||
            ImGui::Button("Reload")
        ){
            needsUpdate = true;
        }
        ImGui::End();
        return needsUpdate;
    }


    SDL_Surface* createSurfaceFromNoise(){
        int imgW = size.w; int imgH = size.h;
        Uint8 pixelsData[imgW*imgH*4];
        const siv::PerlinNoise perlin{(siv::PerlinNoise::seed_type)seed};
        for (int j = 0; j < imgH; j+=1) {
            for (int i = 0; i < imgW; i+=1) {
                //Vertical Gradient
                const float fImgH = (float)imgH;
                const float vertGradientSize = fImgH*vertGradientRatio;
                const float vertGradientGap = (fImgH - vertGradientSize)/2.0f;
                float vertGradientVal = (j-vertGradientGap)/vertGradientSize;
                float verticalGradient_01 = (j < vertGradientGap) ? 0.0f : ((j > vertGradientSize + vertGradientGap) ? 1.0f : vertGradientVal);

                //Noise 2D
                const double noise2D_01 = perlin.octave2D_01((i * scale.x), (j * scale.y), octaves, persistance);

                //Noise 1D
                const double noise1D_01 = perlin.octave1D_01((i * scale.x), octaves);

                //Group and finalize
                double final_01 = (noise2D_01 + noise1D_01 + verticalGradient_01*3)/5.0f;
                if (multiplyGradient){final_01 = (noise2D_01 * noise1D_01 * verticalGradient_01);}

                Uint8 pixelVal = floor(final_01 * 255.0f);
                if (binaryOutput){pixelVal = (pixelVal > binaryThreshold) ? 255 : 0;}
                pixelsData[(i+(imgH*j))*4 + 0] = pixelVal;
                pixelsData[(i+(imgH*j))*4 + 1] = pixelVal;
                pixelsData[(i+(imgH*j))*4 + 2] = pixelVal;
                pixelsData[(i+(imgH*j))*4 + 3] = (Uint8)255;
            }
        }
        return SDL_CreateRGBSurfaceFrom(pixelsData, imgW, imgH, 32, imgW*4, 0, 0, 0, 0);
    }
};




#endif // UTILS_MATH_H_
