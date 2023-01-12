#ifndef RENDERER_H_
#define RENDERER_H_

#include <cstddef>
#include <iostream>
#include <string>
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "Utils_geometry.hpp"
#include "SDL_ttf.h"

class Scene;
extern float global_avgFPS;

struct Texture{
    SDL_Texture* sdlTexture = nullptr;
    size_t w = 0;
    size_t h = 0;

    Texture() = default;
    void free(){
        SDL_DestroyTexture(this->sdlTexture);
        w = 0; h = 0;
    }

    Texture(SDL_Surface *s, SDL_Renderer* sdlRenderer) {
      this->sdlTexture = SDL_CreateTextureFromSurface(sdlRenderer, s);
      this->w = s->w;
      this->h = s->h;
      std::cout << "Creating texture with size: " << w << " " << h << std::endl;
    }
};

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
        void drawText(int x, int y, std::string text, SDL_Color col);
        void drawCircle(int cx, int cy, int radius, SDL_Color col);
        void drawTexture(const Texture& tex, int x, int y);

    private:
        iSize screenRes;
        TTF_Font* currentFont = nullptr;
        SDL_Window* sdlWindow = nullptr;
        SDL_Renderer* sdlRenderer = nullptr;

        Texture debugTexture;

        bool imgui_showDemo = true;
};



#endif // RENDERER_H_
