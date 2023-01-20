#ifndef UTILS_MATH_H_
#define UTILS_MATH_H_

#include <algorithm>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

#ifdef __APPLE__
#include <sys/_types/_size_t.h>
#endif

#include "PerlinNoise.hpp"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "Utils_points.hpp"
#include "imgui.h"
#include "imgui_curves.hpp"

inline int randInt(int min, int max){
    return rand() % (max-min+1) + min;
}

inline float randFloat(){
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

inline float randFloat(float min, float max){
    return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}

struct InteractiveCurve{
    ImVec2 values[10];
    int selection = -1;
    InteractiveCurve(){values[0].x = ImGui::CurveTerminator;}

    void setContinentalness(){
        values[0] = {0.000000f,0.020000f};
        values[1] = {0.056000f,0.045000f};
        values[2] = {0.158000f,0.080000f};
        values[3] = {0.208000f,0.355000f};
        values[4] = {0.228000f,0.370000f};
        values[5] = {0.520000f,0.425000f};
        values[6] = {0.644000f,0.650000f};
        values[7] = {0.776000f,0.775000f};
        values[8] = {0.876000f,0.940000f};
        values[9] = {1.000000f,1.000000f};
    }

    void setErosion(){
        values[0] = {0.000000f,1.000000f};
        values[1] = {0.054000f,0.910000f};
        values[2] = {0.110000f,0.965000f};
        values[3] = {0.200000f,0.725000f};
        values[4] = {0.414000f,0.600000f};
        values[5] = {0.542000f,0.300000f};
        values[6] = {0.758000f,0.210000f};
        values[7] = {0.826000f,0.255000f};
        values[8] = {0.862000f,0.055000f};
        values[9] = {1.000000f,0.000000f};
    }

    bool renderGUI() {
        if(ImGui::Curve("", ImVec2(500, 200), 10, values, &selection)){
            this->print();
            return true;
        }else{
            return false;
        }
    }

    void print(){
        std::cout << "Curve: ";
        for (int i = 0; i < 10; i++) {
            std::cout << "values["<<std::to_string(i)<<"] = {" << std::to_string(values[i].x) << "f," << std::to_string(values[i].y) << "f};"<<std::endl;
        }
        std::cout << std::endl;
    }

    float getValue(float x){
        return ImGui::CurveValue(x, 10, values);
    }
};


struct DebugSurfaces{
    SDL_Surface* finalSurface = nullptr;
    SDL_Surface* contSurface = nullptr;
    SDL_Surface* erosionSurface = nullptr;

    void free(){
        if (finalSurface != nullptr) { SDL_FreeSurface(finalSurface); finalSurface = nullptr; }
        if (contSurface != nullptr) { SDL_FreeSurface(contSurface); contSurface = nullptr; }
        if (erosionSurface != nullptr) { SDL_FreeSurface(erosionSurface); erosionSurface = nullptr; }
    }
};

class NoiseSurfaceGenerator{
  public:
    fSize size = {1200,400};
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



    InteractiveCurve continentalnessCurve;
    float continentalnessScale = 0.4f;
    int continentalnessOctaves = 7;
    float continentalnessBias = 1.0f;

    InteractiveCurve erosionCurve;
    float erosionScale = 0.3f;
    int erosionOctaves = 7;
    float erosionBias = 0.5f;


    //InteractiveCurve erosionCurve;
    //InteractiveCurve peakAndValleyCurve;



    enum FeatureType {NOISE_1D, NOISE_2D, VERTGRAD};


    NoiseSurfaceGenerator(){
        continentalnessCurve.setContinentalness();
        erosionCurve.setErosion();
    }
    ~NoiseSurfaceGenerator(){}

    bool renderGUI(){
        bool upd = false;
        //Debug Texture
        ImGui::Begin("Debug Texture");
        ImGui::Text("Debug Texture");
        ImGui::Separator();

        upd = ImGui::SliderFloat("SizeX",      &size.w,      100.0f, 2000.0f) || upd;
        upd = ImGui::SliderFloat("SizeY",      &size.h,      100.0f, 1000.0f) || upd;

        ImGui::Separator();
        ImGui::Text("Continentalness");
        upd = ImGui::SliderFloat("ContScale", &continentalnessScale, 0.01f, 10.0f) || upd;
        upd = ImGui::SliderInt("ContOctaves", &continentalnessOctaves, 1, 10) || upd;
        upd = continentalnessCurve.renderGUI() || upd;

        ImGui::Separator();
        ImGui::Text("Erosion");
        upd = ImGui::SliderFloat("EroScale", &erosionScale, 0.01f, 10.0f) || upd;
        upd = ImGui::SliderInt("EroOctaves", &erosionOctaves, 1, 10) || upd;
        upd = erosionCurve.renderGUI() || upd;

        ImGui::Separator();
        ImGui::Text("Bias");
        upd = ImGui::SliderFloat("Continentalness bias", &continentalnessBias, 0.0f, 5.0f) || upd;
        upd = ImGui::SliderFloat("Erosion bias", &erosionBias, 0.0f, 5.0f) || upd;
        ImGui::Separator();

        ImGui::Separator();
        if (ImGui::Button("Randomize")){upd = true; seed = randInt(0, 999999);}
        if (ImGui::Button("Reset")){upd = true; continentalnessCurve.setContinentalness(); erosionCurve.setErosion();}
        ImGui::End();
        return upd;
    }


    bool renderGUI_OLD(){
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


    DebugSurfaces createSurfaceFromNoise(){
        int imgW = floor(size.w); int imgH = floor(size.h);
        SDL_Surface* s = SDL_CreateRGBSurface(0, imgW, imgH, 32, 0, 0, 0, 0);
        Uint8* pixelsData = (Uint8*)s->pixels;

        SDL_Surface* contSurf = SDL_CreateRGBSurface(0, imgW, 32, 32, 0, 0, 0, 0);
        SDL_Surface* eroSurf = SDL_CreateRGBSurface(0, imgW, 32, 32, 0, 0, 0, 0);
        Uint8* contData = (Uint8*)contSurf->pixels;
        Uint8* eroData = (Uint8*)eroSurf->pixels;

        const int max_iter = 4;
        for (int iter = 0; iter < max_iter; iter++){
            const siv::PerlinNoise perlin{(siv::PerlinNoise::seed_type)(seed + (iter+1))};
            float MAX_HEIGHT = imgH;
            float HALF_HEIGHT = MAX_HEIGHT/2.0f;
            for (int i = 0; i < imgW; i+=1) {
                //Continentalness, how fat inland we are
                float CONT_NOISE = perlin.octave1D_01((i * continentalnessScale/100.0f), continentalnessOctaves);
                CONT_NOISE = (CONT_NOISE/continentalnessBias) + ((continentalnessBias-1)/continentalnessBias); // Apply Bias
                float curveValue_01 = continentalnessCurve.getValue(CONT_NOISE); // Apply Spline
                curveValue_01 = (1.0f - curveValue_01); //Flip values
                float CONT_HEIGHT = ((curveValue_01 - 0.5f) * 2.0f) * (HALF_HEIGHT*0.8f); // first to -1 <-> +1 and then upscaled to HEIGHT

                //Erosion, Vertical Gradient squashing everything with high values of erosion
                float ERO_NOISE = perlin.octave1D_01((i * erosionScale/100.0f)+9999.0f, erosionOctaves);
                ERO_NOISE = (ERO_NOISE/erosionBias) + ((erosionBias-1)/erosionBias); // Apply Bias
                float eroValue_01 = erosionCurve.getValue(ERO_NOISE); // Apply Spline
                float ERO_VALUE_01 = (eroValue_01); //high value = high erosion

                //Final height
                float height = HALF_HEIGHT + CONT_HEIGHT * ERO_VALUE_01;
                for (int j = 0; j < imgH; j+=1) {
                    Uint8 val = 255/(max_iter - iter) ;
                    Uint8 c = (j>height) ? val : 0;
                    if (c > 0){
                        pixelsData[(i+(imgW*j))*4 + 0] = c;
                        pixelsData[(i+(imgW*j))*4 + 1] = c;
                        pixelsData[(i+(imgW*j))*4 + 2] = c;
                        pixelsData[(i+(imgW*j))*4 + 3] = 255;
                    }
                }

                if (iter == max_iter-1){
                    //Continentalness
                    for (int j = 0; j < 32; j+=1) {
                        Uint8 c = CONT_NOISE*255.0f;
                        contData[(i+(imgW*j))*4 + 0] = c;
                        contData[(i+(imgW*j))*4 + 1] = c;
                        contData[(i+(imgW*j))*4 + 2] = c;
                        contData[(i+(imgW*j))*4 + 3] = 255;
                    }

                    //Erosion
                    for (int j = 0; j < 32; j+=1) {
                        Uint8 c = ERO_NOISE*255.0f;
                        eroData[(i+(imgW*j))*4 + 0] = c;
                        eroData[(i+(imgW*j))*4 + 1] = c;
                        eroData[(i+(imgW*j))*4 + 2] = c;
                        eroData[(i+(imgW*j))*4 + 3] = 255;
                    }
                }
            }
        }

        DebugSurfaces db = DebugSurfaces();
        db.finalSurface = s;
        db.contSurface = contSurf;
        db.erosionSurface = eroSurf;
        return db;
    }

    SDL_Surface* createSurfaceFromNoise_OLD(){
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
