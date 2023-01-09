#ifndef RENDERER_H_
#define RENDERER_H_

#include <string>
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "Utils_geometry.hpp"
#include "Scene.hpp"
#include "SDL_ttf.h"

extern float global_avgFPS;

class Renderer {
    public:
        Renderer(SDL_Window* sdlWindow, iSize screenRes);
        virtual ~Renderer();

        //Render Events
        void renderFrameBegin();
        void renderScene(const Scene& s);
        void renderGUI(Scene& s);
        void renderFrameEnd();

        //Drawing operations
        void drawLine(int x1, int y1, int x2, int y2, SDL_Color col);
        void drawText(int x, int y, std::string text, SDL_Color col);
        void drawCircle(int cx, int cy, int radius, SDL_Color col);

    private:
        iSize screenRes;
        TTF_Font* currentFont = nullptr;
        SDL_Window* sdlWindow = nullptr;
        SDL_Renderer* sdlRenderer = nullptr;

        bool imgui_showDemo = true;
};


#endif // RENDERER_H_
