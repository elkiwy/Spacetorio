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
#include "TextureManager.hpp"

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

        //OpenGL Data Management
        void setupLinesVAO();
        void setupTilesVAO();
        void setupSpritesVAO();
        void updateRenderableTilesVBO(std::vector<TileRenderData>& tilesData);
        void updateRenderableSpritesVBO(std::vector<SpriteRenderData>& spritesData);

        void addLineToRender(const fPoint& p1, const fPoint& p2, SDL_Color c = {255,0,128,0});
        void addRectToRenderP1P2(const fPoint& p1, const fPoint& p2, SDL_Color c = {255,0,128,0});
        void addRectToRenderCentered(const fPoint& center, const fSize& size, SDL_Color c = {255,0,128,0});
        void _updateLinesVBO();


        glm::uint _loadTextureFromFile(const std::string& path);
        glm::uint _loadTextureFromSurface(SDL_Surface* s);

        //Drawing operations
        void drawLine(int x1, int y1, int x2, int y2, SDL_Color col);
        void drawRect(int cx, int cy, int w, int h, SDL_Color col);
        void drawRectFill(int cx, int cy, int w, int h, SDL_Color col);
        void drawText(int x, int y, std::string text, SDL_Color col);
        void drawCircle(int cx, int cy, int radius, SDL_Color col);
        void drawTexture(const Texture& tex, int cx, int cy, float angleDeg = 0.0f, float scale = 1.0f);

        SDL_Renderer* getSdlRenderer(){return sdlRenderer;};
        iSize getScreenSize(){return screenRes;};

        TextureManager& getTextureManager(){return textureManager;};

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


        //Texture Manager
        TextureManager textureManager;

        //Tile rendering
        Shader tileShader;
        glm::uint abstractTileVBO = 0;
        glm::uint abstractTileVAO = 0;
        glm::uint renderableTilesVBO = 0;
        int tilesToRender = 0;
        glm::uint tilesTextureId = 0;
        glm::uint tilesTextureId2 = 0;

        //Sprite rendering
        Shader spriteShader;
        glm::uint spriteVBO = 0;
        glm::uint spriteVAO = 0;
        int spritesToRender = 0;

        //Lines rendering
        Shader basicShader;
        glm::uint linesVBO = 0;
        glm::uint linesVAO = 0;
        std::vector<LineRenderData> linesRenderData;
        int linesToRender = 0;


        glm::mat4 transformMatrix = glm::mat4(1.0f);
};



#endif // RENDERER_H_
