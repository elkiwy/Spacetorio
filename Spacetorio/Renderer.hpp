#ifndef RENDERER_H_
#define RENDERER_H_

#include <cstddef>
#include <iostream>
#include <string>
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "Utils_points.hpp"
#include "Utils_math.hpp"
#include "SDL_ttf.h"

#include "Texture.hpp"

class Scene;
extern float global_avgFPS;


class Renderer {
    public:
        Renderer(SDL_Window* sdlWindow, iSize screenRes);
        virtual ~Renderer();

        //Render Events
        void renderFrameBegin();
        void renderScene(Scene& s);
        void renderGUI(Scene& s);
        void renderFrameEnd();

        //Drawing operations
        void drawLine(int x1, int y1, int x2, int y2, SDL_Color col);
        void drawRect(int cx, int cy, int w, int h, SDL_Color col);
        void drawRectFill(int cx, int cy, int w, int h, SDL_Color col);
        void drawText(int x, int y, std::string text, SDL_Color col);
        void drawCircle(int cx, int cy, int radius, SDL_Color col);
        void drawTexture(const Texture& tex, int cx, int cy, float angle = 0.0f, float scale = 1.0f);

        SDL_Renderer* getSdlRenderer(){return sdlRenderer;};

    private:
        iSize screenRes;
        TTF_Font* currentFont = nullptr;
        SDL_Window* sdlWindow = nullptr;
        SDL_Renderer* sdlRenderer = nullptr;

        NoiseSurfaceGenerator gen;
        Texture debugTexture;

        bool imgui_showDemo = true;
};



#endif // RENDERER_H_
