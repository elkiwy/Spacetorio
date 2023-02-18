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

enum SplineCurveTemplates {CURVE_CONTINENTALNESS, CURVE_EROSION, CURVE_CAVES};

struct SplineCurve{
    ImVec2 values[10];
    int selection = -1;

    SplineCurve(){values[0].x = ImGui::CurveTerminator;}
    SplineCurve(SplineCurveTemplates temp) : SplineCurve() {
        if (temp == CURVE_CONTINENTALNESS) { setContinentalness(); }
        else if (temp == CURVE_EROSION) { setErosion(); }
        else if (temp == CURVE_CAVES) { setCaves(); }
    }

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

    void setCaves() {

        values[0] = {0.000000f, 0.000000f};
        values[1] = {0.134000f, 1.000000f};
        values[2] = {1.000000f, 1.000000f};
        values[3] = {-10000.000000f, 1.000000f};
        values[4] = {-10000.000000f, 1.000000f};
        values[5] = {-10000.000000f, 1.000000f};
        values[6] = {-10000.000000f, 1.000000f};
        values[7] = {-10000.000000f, 1.000000f};
        values[8] = {-10000.000000f, 1.000000f};
        values[9] = {-10000.000000f, 1.000000f};
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

    float getValue(float x) const{
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
    struct GenerationSettings{
        float scale_cont = 0.3f;
        float bias_cont = 1.0f;
        int octaves_cont = 7;
        SplineCurve curve_cont = SplineCurve(CURVE_CONTINENTALNESS);

        float scale_ero = 0.3f;
        float bias_ero = 0.5f;
        int octaves_ero = 7;
        SplineCurve curve_ero = SplineCurve(CURVE_EROSION);

        float scale_features = 1.5f;
        int octaves_features = 4;
        float persistance_features = 0.5f;

        float scale_caves = 0.75f;
        float bias_caves = 1.0f;
        int octaves_caves = 7;
        float persistance_caves = 0.45f;
        SplineCurve curve_caves = SplineCurve(CURVE_CAVES);
        float caves_gradient = 1.0f;
        bool caves_selection = 1.0f;

        int seed = 0;
    };

    GenerationSettings instanceSettings;

    NoiseSurfaceGenerator(){}

    ~NoiseSurfaceGenerator(){}

    bool renderGUI(){
        bool upd = false;
        ImGui::Begin("Debug Texture");
        ImGui::Text("Debug Texture");

        ImGui::Separator();
        ImGui::Text("Continentalness");
        upd = ImGui::SliderFloat("ContScale", &instanceSettings.scale_cont, 0.01f, 10.0f) || upd;
        upd = ImGui::SliderInt("ContOctaves", &instanceSettings.octaves_cont, 1, 10) || upd;
        upd = instanceSettings.curve_cont.renderGUI() || upd;

        ImGui::Separator();
        ImGui::Text("Erosion");
        upd = ImGui::SliderFloat("EroScale", &instanceSettings.scale_ero, 0.01f, 10.0f) || upd;
        upd = ImGui::SliderInt("EroOctaves", &instanceSettings.octaves_ero, 1, 10) || upd;
        upd = instanceSettings.curve_ero.renderGUI() || upd;

        ImGui::Separator();
        ImGui::Text("Features");
        upd = ImGui::SliderFloat("Scale", &instanceSettings.scale_features, 0.01f, 10.0f) || upd;
        upd = ImGui::SliderInt("Octaves", &instanceSettings.octaves_features, 1, 10) || upd;
        upd = ImGui::SliderFloat("Persistance", &instanceSettings.persistance_features, 0.01f, 1.0f) || upd;

        ImGui::Separator();
        ImGui::Text("Bias");
        upd = ImGui::SliderFloat("Continentalness bias", &instanceSettings.bias_cont, 0.0f, 5.0f) || upd;
        upd = ImGui::SliderFloat("Erosion bias", &instanceSettings.bias_ero, 0.0f, 5.0f) || upd;

        ImGui::Separator();
        ImGui::Text("Caves");
        upd = ImGui::SliderFloat("Scale", &instanceSettings.scale_caves, 0.01f, 10.0f) || upd;
        upd = ImGui::SliderInt("Octaves", &instanceSettings.octaves_caves, 1, 10) || upd;
        upd = ImGui::SliderFloat("Persistance", &instanceSettings.persistance_caves, 0.01f, 1.0f) || upd;
        upd = ImGui::SliderFloat("Caves gradient", &instanceSettings.caves_gradient, 0.0f, 1.0f) || upd;
        upd = ImGui::Checkbox("Caves Selection", &instanceSettings.caves_selection) || upd;
        upd = instanceSettings.curve_caves.renderGUI() || upd;

        ImGui::Separator();
        if (ImGui::Button("Randomize")){upd = true; instanceSettings.seed = randInt(0, 999999);}
        if (ImGui::Button("Reset")){upd = true; instanceSettings.curve_cont.setContinentalness(); instanceSettings.curve_ero.setErosion();}
        ImGui::End();
        return upd;
    }

    SDL_Surface* generateTerrain(iSize res, int backgroundCount, const GenerationSettings& conf, DebugSurfaces* debugSurfaces = nullptr){
        int imgW = res.w; int imgH = res.h;
        SDL_Surface* s = SDL_CreateRGBSurface(0, imgW, imgH, 32, 0, 0, 0, 0);
        Uint8* pixelsData = (Uint8*)s->pixels;

        //Create debug surfaces
        SDL_Surface* contSurf = nullptr;
        SDL_Surface* eroSurf = nullptr;
        Uint8* contData = nullptr;
        Uint8* eroData = nullptr;
        if (debugSurfaces != nullptr){
            contSurf = SDL_CreateRGBSurface(0, imgW, 32, 32, 0, 0, 0, 0);
            eroSurf = SDL_CreateRGBSurface(0, imgW, 32, 32, 0, 0, 0, 0);
            contData = (Uint8*)contSurf->pixels;
            eroData = (Uint8*)eroSurf->pixels;
        }

        const int max_iter = backgroundCount+1;
        for (int iter = 0; iter < max_iter; iter++){
            const siv::PerlinNoise perlin{(siv::PerlinNoise::seed_type)(conf.seed + (iter+1))};
            float MAX_HEIGHT = imgH;
            float HALF_HEIGHT = MAX_HEIGHT/2.0f;
            for (int i = 0; i < imgW; i+=1) {
                //Continentalness, how fat inland we are
                float CONT_NOISE = perlin.octave1D_01((i * conf.scale_cont/100.0f), conf.octaves_cont);
                CONT_NOISE = (CONT_NOISE/conf.bias_cont) + ((conf.bias_cont-1)/conf.bias_cont); // Apply Bias
                float curveValue_01 = conf.curve_cont.getValue(CONT_NOISE); // Apply Spline
                curveValue_01 = (1.0f - curveValue_01); //Flip values
                float CONT_HEIGHT = ((curveValue_01 - 0.5f) * 2.0f) * (HALF_HEIGHT*0.8f); // first to -1 <-> +1 and then upscaled to HEIGHT

                //Erosion, Vertical Gradient squashing everything with high values of erosion
                float ERO_NOISE = perlin.octave1D_01((i * conf.scale_ero/100.0f)+9999.0f, conf.octaves_ero);
                ERO_NOISE = (ERO_NOISE/conf.bias_ero) + ((conf.bias_ero-1)/conf.bias_ero); // Apply Bias
                float eroValue_01 = conf.curve_ero.getValue(ERO_NOISE); // Apply Spline
                float ERO_VALUE_01 = (eroValue_01); //high value = high erosion

                //Final height
                float height = HALF_HEIGHT + CONT_HEIGHT * ERO_VALUE_01;
                for (int j = 0; j < imgH; j+=1) {
                    //Get a gradient value around the 1D height value to be able to mix it up with the 2D noise
                    float gradSize = 20.0f;
                    float diff = fmax(fmin((j - height), gradSize), -gradSize);
                    float gradStep = 128/gradSize;
                    int density_0255 = 128 + (int)(diff * gradStep);

                    //Features - Noise 2D
                    const float FEATURES_NOISE = perlin.octave2D_01((i * (conf.scale_features/100.0f)), (j * (conf.scale_features/100.0f)), conf.octaves_features, conf.persistance_features);
                    density_0255 = fmax((FEATURES_NOISE*2.0f) * (float)density_0255, density_0255);

                    //Caves - Noise 2D
                    float CAVES_NOISE = perlin.octave2D_11((i * (conf.scale_caves/100.0f)), (j * (conf.scale_caves/100.0f)), conf.octaves_caves, conf.persistance_caves);
                    CAVES_NOISE = fabs(CAVES_NOISE); //Ridgedmulti value
                    float caves_value = conf.curve_caves.getValue(CAVES_NOISE); // Apply Spline

                    float caves_gradSize = imgH*0.5;
                    float caves_gradUpper = (imgH - caves_gradSize)*0.5f;
                    float caves_gradLower = caves_gradUpper + caves_gradSize;
                    float caves_gradInfluence = 1.0f; //Caves value influence over the density
                    if (j < caves_gradUpper){ caves_gradInfluence = 0.0f; }
                    else if (j > caves_gradLower){ caves_gradInfluence = 1.0f; }
                    else { caves_gradInfluence = (((float)j - caves_gradUpper) / caves_gradSize); }
                    float density_01 = ((float)density_0255)/255.0f;
                    density_01 = std::lerp(density_01, caves_value*density_01, caves_gradInfluence);

                    density_0255 = std::min((int)(density_01 * 255.0f), 255);

                    //Uint8 pixelValue = (j>height) ? layerColor : 0;
                    Uint8 layerColor = 255/(max_iter - iter) ;
                    bool solidPixel = (density_0255 >= 128);

                    //layerColor = density_0255; //override to show gradients
                    if (solidPixel){
                        pixelsData[(i+(imgW*j))*4 + 0] = layerColor;
                        pixelsData[(i+(imgW*j))*4 + 1] = layerColor;
                        pixelsData[(i+(imgW*j))*4 + 2] = layerColor;
                        pixelsData[(i+(imgW*j))*4 + 3] = 255;
                    }
                }



                if (debugSurfaces != nullptr && iter == max_iter-1){
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

        if (debugSurfaces!=nullptr){
            debugSurfaces->finalSurface = s;
            debugSurfaces->contSurface = contSurf;
            debugSurfaces->erosionSurface = eroSurf;
        }

        return s;
    }

    SDL_Surface* generateTerrain_test(iSize res, int backgroundCount, const GenerationSettings& conf, DebugSurfaces* debugSurfaces = nullptr){
        int imgW = res.w; int imgH = res.h;
        SDL_Surface* s = SDL_CreateRGBSurface(0, imgW, imgH, 32, 0, 0, 0, 0);
        Uint8* pixelsData = (Uint8*)s->pixels;

        //Create debug surfaces
        SDL_Surface* contSurf = nullptr;
        SDL_Surface* eroSurf = nullptr;
        Uint8* contData = nullptr;
        Uint8* eroData = nullptr;
        if (debugSurfaces != nullptr){
            contSurf = SDL_CreateRGBSurface(0, imgW, 32, 32, 0, 0, 0, 0);
            eroSurf = SDL_CreateRGBSurface(0, imgW, 32, 32, 0, 0, 0, 0);
            contData = (Uint8*)contSurf->pixels;
            eroData = (Uint8*)eroSurf->pixels;
        }

        const int max_iter = backgroundCount+1;
        for (int iter = 0; iter < max_iter; iter++){
            const siv::PerlinNoise perlin{(siv::PerlinNoise::seed_type)(conf.seed + (iter+1))};
            float MAX_HEIGHT = imgH;
            float HALF_HEIGHT = MAX_HEIGHT/2.0f;
            for (int i = 0; i < imgW; i+=1) {
                for (int j = 0; j < imgH; j+=1) {
                    //Features - Noise 2D
                    const float FEATURES_NOISE = perlin.octave2D_11((i * (conf.scale_caves/100.0f)), (j * (conf.scale_caves/100.0f)), conf.octaves_caves, conf.persistance_caves);
                    //float ridgedmulti_value = 1.0f-fabs(FEATURES_NOISE);
                    float ridgedmulti_value = fabs(FEATURES_NOISE);

                    //ridgedmulti_value *= conf.caves_mul;
                    ridgedmulti_value = conf.curve_caves.getValue(ridgedmulti_value); // Apply Spline


                    float density_0255 = ridgedmulti_value * 255.0f;
                    if (conf.caves_selection){
                        density_0255 = ((ridgedmulti_value > 0.5f) ? 1.0f : 0.0f) * 255.0f;
                    }

                    //layerColor = density_0255; //override to show gradients
                    pixelsData[(i+(imgW*j))*4 + 0] = density_0255;
                    pixelsData[(i+(imgW*j))*4 + 1] = density_0255;
                    pixelsData[(i+(imgW*j))*4 + 2] = density_0255;
                    pixelsData[(i+(imgW*j))*4 + 3] = 255;
                }
            }
        }

        if (debugSurfaces!=nullptr){
            debugSurfaces->finalSurface = s;
            debugSurfaces->contSurface = contSurf;
            debugSurfaces->erosionSurface = eroSurf;
        }

        return s;
    }

    SDL_Surface* generateTerrainInstanceSettings(iSize res, int backgroundCount, DebugSurfaces* debugSurfaces = nullptr){
        return this->generateTerrain(res, backgroundCount, instanceSettings, debugSurfaces);
    }

    SDL_Surface* generateTerrainInstanceSettings_test(iSize res, int backgroundCount, DebugSurfaces* debugSurfaces = nullptr){
        return this->generateTerrain(res, backgroundCount, instanceSettings, debugSurfaces);
    }

};


























#endif // UTILS_MATH_H_
