#include "Renderer.hpp"
#include "SDL_pixels.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "SDL_ttf.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <string>

#include "SDL_video.h"
#include "Scene.hpp"


#include <glm/ext/matrix_transform.hpp>

#include "Utils_math.hpp"
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_sdlrenderer.h"

#include "PerlinNoise.hpp"
#include "Texture.hpp"


#include "GL/glew.h"
#include "SDL_opengl.h"


//#ifdef __APPLE__
#define ASSETS_PREFIX std::string("Spacetorio/")
//#else
//#define ASSETS_PREFIX std::string("")
//#endif

Renderer::Renderer(SDL_Window* sdlWin, iSize sr) : screenRes(sr){
    std::cout << "Renderer initializing..." << std::endl;
    sdlWindow = sdlWin;

    //Predefine GL Versions and attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //Initialize GL Context
    this->glContext = SDL_GL_CreateContext(sdlWindow);
    if (this->glContext == NULL){
        std::cout << "\n\nCouldn't initialize SDL_GL_Context, probably will crash soon!"<< SDL_GetError() <<"\n\n" << std::endl << std::flush;
    }

    //Initialize GLEW
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) { printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError)); }
    if (SDL_GL_SetSwapInterval(1) < 0) { printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError()); }

    //Prepare transformation matrix to convert from -1 1 to 0 SCREENRES
    this->transformMatrix = glm::mat4(1.0f);
    this->transformMatrix = glm::translate(transformMatrix, glm::vec3(-1.0f, -1.0f, 0.0f));
    this->transformMatrix = glm::scale(transformMatrix, glm::vec3(2.0f/sr.w, 2.0f/sr.h, 1.0f));

    //Initialize Tile Shaders, VAO, and VBO
    this->tileShader = Shader((ASSETS_PREFIX+"shaders/tiles.vert").c_str(), (ASSETS_PREFIX+"shaders/tiles.frag").c_str());
    this->tileShader.use();
    this->tileShader.setMat4("uTransformMatrix", this->transformMatrix);
    this->setupAbstractTileVAO();
    this->updateRenderableTilesVBO();

    //Initialize Fonts (maybe deprecated)
    currentFont = TTF_OpenFont((ASSETS_PREFIX+"res/fonts/DejaVuSansMono.ttf").c_str(), 16);

    ////ImGui Stuff
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGui::StyleColorsDark();
    //ImGui_ImplSDL2_InitForSDLRenderer(sdlWin, sdlRenderer);
    //ImGui_ImplSDLRenderer_Init(sdlRenderer);
    //std::cout << "Renderer Initialized." << std::endl;
}



void Renderer::setupAbstractTileVAO(){
    assert(this->abstractTileVAO == 0 && "ERROR: called setupAbstractTileVAO multiple times!");

    //Setup data
    float s = TILE_SIZE / 2.0f;
    float abstractTileData[] = {
        //Positions     //colors
        -s,  s,   1.0f, 0.0f, 0.0f,
         s, -s,   0.0f, 1.0f, 0.0f,
        -s, -s,   0.0f, 0.0f, 1.0f,

        -s,  s,   1.0f, 0.0f, 0.0f,
         s, -s,   0.0f, 1.0f, 0.0f,
         s,  s,   0.0f, 1.0f, 1.0f
    };

    //Create and bind the VAO and VBO
    glGenVertexArrays(1, &this->abstractTileVAO);
    glGenBuffers(1, &this->abstractTileVBO);
    glBindVertexArray(this->abstractTileVAO);

    //Set the VBO data
    glBindBuffer(GL_ARRAY_BUFFER, this->abstractTileVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(abstractTileData), abstractTileData, GL_STATIC_DRAW);
    
    //Enable 0th input as 2 floats
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    //Enable 1st input as 3 floats
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2*sizeof(float)));


}



void Renderer::updateRenderableTilesVBO(){
    this->tilesToRender = 300;

    //Setup Dummy tiles data
    glm::vec2 tilesData[this->tilesToRender];
    int index = 0;
    float offset = 0.00f;

    {
        glm::vec2 tileData;
        tileData.x = 100.0f;
        tileData.y = 200.0f;
        tilesData[index++] = tileData;
    }
    {
        glm::vec2 tileData;
        tileData.x = 100.0f;
        tileData.y = 500.0f;
        tilesData[index++] = tileData;
    }
    {
        glm::vec2 tileData;
        tileData.x = 100.0f;
        tileData.y = 1000.0f;
        tilesData[index++] = tileData;
    }

    for(int y=0; y<screenRes.h; y+=TILE_SIZE*2){
        for(int x=0; x<screenRes.w; x+=TILE_SIZE){
            glm::vec2 tileData;
            tileData.x = (float)x + offset;
            tileData.y = (float)y + offset + 200;
            if (index<this->tilesToRender){
                tilesData[index++] = tileData;
            }
        }
    }

    if (this->renderableTilesVBO == 0){
        //Create, bind, and set the buffer
        glGenBuffers(1, &this->renderableTilesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, this->renderableTilesVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * this->tilesToRender, &tilesData[0], GL_DYNAMIC_DRAW);

        std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

        //Enabled 2nd input as 2 floats
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, this->renderableTilesVBO);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(2, 1);

    }else{
        //Bind and update the buffer content
        glBindBuffer(GL_ARRAY_BUFFER, this->renderableTilesVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * this->tilesToRender, &tilesData[0]);
    }
    //Unbind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}





Renderer::~Renderer(){
    std::cout << "Renderer destroying..." << std::endl;
    debugTextureFinal.free();
    debugTextureContinentalness.free();
    debugTextureErosion.free();

    //ImGui_ImplSDLRenderer_Shutdown();
    //ImGui_ImplSDL2_Shutdown();
    //ImGui::DestroyContext();

    TTF_CloseFont(currentFont);
    SDL_DestroyRenderer(sdlRenderer);
    currentFont = NULL;
    sdlRenderer = NULL;
    std::cout << "Renderer destroyed." << std::endl;
}

void renderTest(SDL_Renderer* sdlRenderer, int SCREEN_WIDTH, int SCREEN_HEIGHT){
    //// Render red filled quad
    //SDL_Rect fillRect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    //SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0x00, 0x00, 0xFF);
    //SDL_RenderFillRect(sdlRenderer, &fillRect);
    // Render green outlined quad
    //SDL_Rect outlineRect = {SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3};
    //SDL_SetRenderDrawColor(sdlRenderer, 0x00, 0xFF, 0x00, 0xFF);
    //SDL_RenderDrawRect(sdlRenderer, &outlineRect);
    //// Draw blue horizontal line
    //SDL_SetRenderDrawColor(sdlRenderer, 0x00, 0x00, 0xFF, 0xFF);
    //SDL_RenderDrawLine(sdlRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
    //// Draw vertical line of yellow dots
    //SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0x00, 0xFF);
    //for (int i = 0; i < SCREEN_HEIGHT; i += 4) {
    //    SDL_RenderDrawPoint(sdlRenderer, SCREEN_WIDTH / 2, i);
    //}
}



/*
** Render Events
 */

void Renderer::renderFrameBegin(){
    //Clear screen
    SDL_SetRenderDrawColor(sdlRenderer, 0x80, 0x80, 0x80, 0xFF);
    SDL_RenderClear(sdlRenderer);
}

void Renderer::renderScene(Scene* s){

    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    tileShader.use();
    tileShader.setMat4("uCameraMatrix", s->getCamera().getCameraMatrix());

    glBindVertexArray(this->abstractTileVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, this->tilesToRender);
    glBindVertexArray(0);

    //renderTest(sdlRenderer, screenRes.w, screenRes.h);
    //s->render();

    ////Noise Generator debug
    //if (debugTextureFinal.initialized){
    //    drawTexture(debugTextureFinal, debugTextureFinal.w/2.0f+10.0f, debugTextureFinal.h/2.0f+10.0f);
    //    drawTexture(debugTextureContinentalness, debugTextureContinentalness.w/2.0f+10.0f, debugTextureContinentalness.h/2.0f+10.0f + debugTextureFinal.h + 10.0f);
    //    drawTexture(debugTextureErosion, debugTextureErosion.w/2.0f+10.0f, debugTextureErosion.h/2.0f+10.0f + debugTextureFinal.h + 10.0f + debugTextureContinentalness.h + 10.0f);
    //}
}

void Renderer::renderGUI(Scene* s){
    //ImGui_ImplSDLRenderer_NewFrame();
    //ImGui_ImplSDL2_NewFrame();
    //ImGui::NewFrame();

    ////Scene GUI
    //s->renderGUI();

    //////Noise Generator debug
    ////if (gen.renderGUI() || debugTextureFinal.initialized == false){
    ////    DebugSurfaces ds;
    ////    gen.generateTerrainInstanceSettings({1200,400}, 3, &ds);
    ////    debugTextureFinal = Texture(ds.finalSurface, false);
    ////    debugTextureContinentalness = Texture(ds.contSurface, false);
    ////    debugTextureErosion = Texture(ds.erosionSurface, false);
    ////    ds.free();
    ////}

    ////Rendere and complete ImGui
    //ImGui::Render();
    //ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::renderFrameEnd(){
    //Update the image on screen
    drawText(8, 8, "FPS: " + std::to_string(global_avgFPS), {0x00, 0x00, 0x00, 0xff});
    //SDL_RenderPresent(sdlRenderer);
    SDL_GL_SwapWindow(sdlWindow);
}




/*
** Drawing operations
*/

void Renderer::drawLine(int x1, int y1, int x2, int y2, SDL_Color col){
    Uint8 prevR; Uint8 prevG; Uint8 prevB; Uint8 prevA;
    SDL_GetRenderDrawColor(sdlRenderer, &prevR, &prevG, &prevB, &prevA);
    SDL_SetRenderDrawColor(sdlRenderer, col.r, col.g, col.b, col.a);
    SDL_RenderDrawLine(sdlRenderer, x1, y1, x2, y2);
    SDL_SetRenderDrawColor(sdlRenderer, prevR, prevG, prevB, prevA);
}

void Renderer::drawRect(int cx, int cy, int w, int h, SDL_Color col){
    Uint8 prevR; Uint8 prevG; Uint8 prevB; Uint8 prevA;
    SDL_GetRenderDrawColor(sdlRenderer, &prevR, &prevG, &prevB, &prevA);
    SDL_SetRenderDrawColor(sdlRenderer, col.r, col.g, col.b, col.a);
    SDL_Rect fillRect = {cx-w/2, cy-h/2, w, h};
    SDL_RenderDrawRect(sdlRenderer, &fillRect);
    SDL_SetRenderDrawColor(sdlRenderer, prevR, prevG, prevB, prevA);
}

void Renderer::drawRectFill(int cx, int cy, int w, int h, SDL_Color col){
    Uint8 prevR; Uint8 prevG; Uint8 prevB; Uint8 prevA;
    SDL_GetRenderDrawColor(sdlRenderer, &prevR, &prevG, &prevB, &prevA);
    SDL_SetRenderDrawColor(sdlRenderer, col.r, col.g, col.b, col.a);
    SDL_Rect fillRect = {cx-w/2, cy-h/2, w, h};
    SDL_RenderFillRect(sdlRenderer, &fillRect);
    SDL_SetRenderDrawColor(sdlRenderer, prevR, prevG, prevB, prevA);
}

void Renderer::drawText(int x, int y, std::string text, SDL_Color col){
    SDL_Surface* textSurf = TTF_RenderText_Solid(currentFont, text.c_str(), col);

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(sdlRenderer, textSurf);
    iSize textSize = {textSurf->w, textSurf->h};
    SDL_FreeSurface(textSurf);

    SDL_Rect dstRect = {x, y, textSize.w, textSize.h};
    SDL_RenderCopyEx(sdlRenderer, textTexture, NULL, &dstRect, 0, NULL, SDL_FLIP_NONE);
    SDL_DestroyTexture(textTexture);
}

void Renderer::drawTexture(const Texture& t, int cx, int cy, float angleDeg, float scale){
    int scaledW = t.w*scale;
    int scaledH = t.h*scale;
    int ULCornerX = cx - scaledW/2.0f;
    int ULCornerY = cy - scaledH/2.0f;

    SDL_Rect dstRect = {ULCornerX, ULCornerY, scaledW, scaledH};
    SDL_RenderCopyEx(sdlRenderer, t.sdlTexture, NULL, &dstRect, angleDeg, NULL, SDL_FLIP_NONE);
}

void Renderer::drawCircle(int cx, int cy, int radius, SDL_Color col){
    const int32_t diameter = (radius * 2);
    int32_t x = (radius - 1); int32_t y = 0;
    int32_t tx = 1; int32_t ty = 1;
    int32_t error = (tx - diameter);
    Uint8 prevR; Uint8 prevG; Uint8 prevB; Uint8 prevA;
    SDL_GetRenderDrawColor(sdlRenderer, &prevR, &prevG, &prevB, &prevA);
    SDL_SetRenderDrawColor(sdlRenderer, col.r, col.g, col.b, col.a);
    while (x >= y) {
        SDL_RenderDrawPoint(sdlRenderer, cx + x, cy - y);
        SDL_RenderDrawPoint(sdlRenderer, cx + x, cy + y);
        SDL_RenderDrawPoint(sdlRenderer, cx - x, cy - y);
        SDL_RenderDrawPoint(sdlRenderer, cx - x, cy + y);
        SDL_RenderDrawPoint(sdlRenderer, cx + y, cy - x);
        SDL_RenderDrawPoint(sdlRenderer, cx + y, cy + x);
        SDL_RenderDrawPoint(sdlRenderer, cx - y, cy - x);
        SDL_RenderDrawPoint(sdlRenderer, cx - y, cy + x);
        if (error <= 0) {++y; error += ty; ty += 2;}
        if (error > 0) {--x; tx += 2; error += (tx - diameter);}
    }
    SDL_SetRenderDrawColor(sdlRenderer, prevR, prevG, prevB, prevA);
}
