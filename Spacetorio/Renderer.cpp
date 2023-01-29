#include "Renderer.hpp"

//SDL Stuff
#include "SDL_pixels.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "SDL_ttf.h"
#include "SDL_video.h"

//STD stuff
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <string>

//OpenGL Stuff
#include "GL/glew.h"
#include "SDL_opengl.h"

//GLM stuff
#include <glm/ext/matrix_transform.hpp>

//ImGui stuff
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"

//Spacetorio stuff
#include "Scene.hpp"
#include "Utils_math.hpp"
#include "PerlinNoise.hpp"
#include "Texture.hpp"




//#ifdef __APPLE__
#define ASSETS_PREFIX std::string("Spacetorio/")
//#else
//#define ASSETS_PREFIX std::string("")
//#endif

Renderer::Renderer(SDL_Window* sdlWin, iSize sr) : screenRes(sr){
    std::cout << "Renderer initializing..." << std::endl;
    sdlWindow = sdlWin;

    //Predefine GL Versions and attributes
    const char* glsl_version = "#version 330";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    //Initialize GL Context
    this->glContext = SDL_GL_CreateContext(sdlWindow);
    if (this->glContext == NULL){
        std::cout << "\n\nCouldn't initialize SDL_GL_Context, probably will crash soon!"<< SDL_GetError() <<"\n\n" << std::endl << std::flush;
    }
    SDL_GL_MakeCurrent(sdlWin, this->glContext);
    if (SDL_GL_SetSwapInterval(1) < 0) { printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError()); }


    //Initialize GLEW
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) { printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError)); }
    std::cout << "Initialized OpenGL with version: " << glGetString(GL_VERSION) << std::endl;

    //Prepare transformation matrix to convert from -1 1 to 0 SCREENRES
    this->transformMatrix = glm::mat4(1.0f);
    this->transformMatrix = glm::translate(transformMatrix, glm::vec3(-1.0f, -1.0f, 0.0f));
    this->transformMatrix = glm::scale(transformMatrix, glm::vec3(2.0f/sr.w, 2.0f/sr.h, 1.0f));

    //Initialize Tile Shaders, VAO, and VBO
    this->tileShader = Shader((ASSETS_PREFIX+"shaders/tiles.vert").c_str(), (ASSETS_PREFIX+"shaders/tiles.frag").c_str());
    this->tileShader.use();
    this->tileShader.setMat4("uTransformMatrix", this->transformMatrix);
    this->setupAbstractTileVAO();
    //this->updateRenderableTilesVBO();

    //Initialize Fonts (maybe deprecated)
    currentFont = TTF_OpenFont((ASSETS_PREFIX+"res/fonts/DejaVuSansMono.ttf").c_str(), 16);

    //ImGui Stuff
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(this->sdlWindow, this->glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
    std::cout << "Renderer Initialized." << std::endl;
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



void Renderer::updateRenderableTilesVBO(std::vector<TileRenderData>& tilesData){
    //NOTE: could be better to create a larger buffer and only call glBufferSubData instead of calling glBufferData to resize it

    if (tilesData.size() == 0){
        return;
    }

    //Update the count of how many tiles to render
    this->tilesToRender = tilesData.size();
    glm::vec2* rawData = new glm::vec2[tilesToRender];
    int index = 0;

    //Fill in the raw data to send the gpu
    for (auto &td : tilesData) {
        glm::vec2 d = {td.pos.x, td.pos.y};
        rawData[index] = d;
        index++;
    }

    // DEBUG:  Reference tile
    //rawData[0].x = 100.0f;
    //rawData[0].y = 500.0f;

    if (this->renderableTilesVBO == 0){
        //Create, bind, and set the buffer
        glGenBuffers(1, &this->renderableTilesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, this->renderableTilesVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * this->tilesToRender, &rawData[0], GL_DYNAMIC_DRAW);

        //Enabled 2nd input as 2 floats
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, this->renderableTilesVBO);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(2, 1);

    }else{
        std::cout << "Updating chunk data with " << this->tilesToRender << " tiles " << std::endl;

        std::cout << "rawData[0] : " << rawData[0].x << " " << rawData[0].y << std::endl;
        std::cout << "rawData[1] : " << rawData[1].x << " " << rawData[1].y << std::endl;
        std::cout << "rawData[2] : " << rawData[2].x << " " << rawData[2].y << std::endl;
        std::cout << "rawData[3] : " << rawData[3].x << " " << rawData[3].y << std::endl;


        glBindBuffer(GL_ARRAY_BUFFER, this->renderableTilesVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * this->tilesToRender, &rawData[0], GL_DYNAMIC_DRAW);
    }

    delete[] rawData;

    //Unbind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}





Renderer::~Renderer(){
    std::cout << "Renderer destroying..." << std::endl;
    debugTextureFinal.free();
    debugTextureContinentalness.free();
    debugTextureErosion.free();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    TTF_CloseFont(currentFont);
    SDL_DestroyRenderer(sdlRenderer);
    currentFont = NULL;
    sdlRenderer = NULL;
    std::cout << "Renderer destroyed." << std::endl;
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
    //Clear buffer
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Prepare the render data for Scene
    s->render();

    //Draw call
    tileShader.use();
    tileShader.setMat4("uCameraMatrix", s->getCamera().getCameraMatrix());
    glBindVertexArray(this->abstractTileVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, this->tilesToRender);
    glBindVertexArray(0);

    ////Noise Generator debug
    //if (debugTextureFinal.initialized){
    //    drawTexture(debugTextureFinal, debugTextureFinal.w/2.0f+10.0f, debugTextureFinal.h/2.0f+10.0f);
    //    drawTexture(debugTextureContinentalness, debugTextureContinentalness.w/2.0f+10.0f, debugTextureContinentalness.h/2.0f+10.0f + debugTextureFinal.h + 10.0f);
    //    drawTexture(debugTextureErosion, debugTextureErosion.w/2.0f+10.0f, debugTextureErosion.h/2.0f+10.0f + debugTextureFinal.h + 10.0f + debugTextureContinentalness.h + 10.0f);
    //}
}

void Renderer::renderGUI(Scene* s){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    //Scene GUI
    s->renderGUI();

    ////Noise Generator debug
    //if (gen.renderGUI() || debugTextureFinal.initialized == false){
    //    DebugSurfaces ds;
    //    gen.generateTerrainInstanceSettings({1200,400}, 3, &ds);
    //    debugTextureFinal = Texture(ds.finalSurface, false);
    //    debugTextureContinentalness = Texture(ds.contSurface, false);
    //    debugTextureErosion = Texture(ds.erosionSurface, false);
    //    ds.free();
    //}

    ImGui::Begin("Renderer");
    ImGui::Text("Renderer");
    ImGui::Text("Tiles count: %d", (int)tilesToRender);
    ImGui::End();

    //Rendere and complete ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::renderFrameEnd(){
    //Update the image on screen
    //drawText(8, 8, "FPS: " + std::to_string(global_avgFPS), {0x00, 0x00, 0x00, 0xff});
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
