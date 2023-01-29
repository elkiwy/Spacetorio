#ifndef RENDERER_H_
#define RENDERER_H_

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_video.h"
#include "Utils_points.hpp"
#include "Utils_math.hpp"
#include "Utils_data.hpp"
#include "SDL_ttf.h"

#include "Texture.hpp"

#include "Renderer_shader.hpp"

#include "glm/glm.hpp"

class Scene;
extern float global_avgFPS;


class Renderer {
    public:
        Renderer(SDL_Window* sdlWindow, iSize screenRes);
        virtual ~Renderer();

        //Render Events
        void renderFrameBegin();
        void renderScene(Scene* s);
        void renderGUI(Scene* s);
        void renderFrameEnd();

        void setupAbstractTileVAO();
        void updateRenderableTilesVBO(std::vector<TileRenderData>& tilesData);

        //Drawing operations
        void drawLine(int x1, int y1, int x2, int y2, SDL_Color col);
        void drawRect(int cx, int cy, int w, int h, SDL_Color col);
        void drawRectFill(int cx, int cy, int w, int h, SDL_Color col);
        void drawText(int x, int y, std::string text, SDL_Color col);
        void drawCircle(int cx, int cy, int radius, SDL_Color col);
        void drawTexture(const Texture& tex, int cx, int cy, float angleDeg = 0.0f, float scale = 1.0f);

        SDL_Renderer* getSdlRenderer(){return sdlRenderer;};
        iSize getScreenSize(){return screenRes;};

    private:
        iSize screenRes;
        TTF_Font* currentFont = nullptr;
        SDL_Window* sdlWindow = nullptr;
        SDL_Renderer* sdlRenderer = nullptr;

        NoiseSurfaceGenerator gen;
        Texture debugTextureFinal;
        Texture debugTextureContinentalness;
        Texture debugTextureErosion;

        bool imgui_showDemo = true;

        SDL_GLContext glContext = nullptr;

        Shader tileShader;
        unsigned int abstractTileVBO = 0;
        unsigned int abstractTileVAO = 0;
        unsigned int renderableTilesVBO = 0;
        int tilesToRender = 0;

        glm::mat4 transformMatrix = glm::mat4(1.0f);
};



#endif // RENDERER_H_
