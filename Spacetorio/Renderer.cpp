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
#include <vector>

//STB stuff
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//ImGui stuff
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"

//Spacetorio stuff
#include "Scene.hpp"
#include "Utils_math.hpp"
#include "PerlinNoise.hpp"
#include "Texture.hpp"
#include "Data_materials.hpp"





Renderer::Renderer(SDL_Window* sdlWin, iSize sr) : screenRes(sr){
    std::cout << "Renderer initializing..." << std::endl;
    sdlWindow = sdlWin;

    //Predefine GL Versions and attributes
    const char* glsl_version = "#version 330";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
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

    //Initialize TextureManger and Texture Atlas
    textureManager.init();
    for (auto& imagePath : getMaterialsSprites()){textureManager.addImage(imagePath);}
    SDL_Surface* textureAtlasSurf = textureManager.createTextureAtlasSurface();
    this->tilesTextureId = this->_loadTextureFromSurface(textureAtlasSurf);
    SDL_FreeSurface(textureAtlasSurf);
    const int samplersCount = 1; int samplers[samplersCount] = {0};

    //Prepare transformation matrix to convert from -1 1 to 0 SCREENRES
    this->transformMatrix = glm::mat4(1.0f);
    this->transformMatrix = glm::translate(transformMatrix, glm::vec3(-1.0f, -1.0f, 0.0f));
    this->transformMatrix = glm::scale(transformMatrix, glm::vec3(2.0f/sr.w, 2.0f/sr.h, 1.0f));

    //Initialize Basic Shaders for debug lines
    this->basicShader = Shader((ASSETS_PREFIX+"shaders/basic.vert").c_str(), (ASSETS_PREFIX+"shaders/basic.frag").c_str());
    this->basicShader.use();
    this->basicShader.setMat4("uTransformMatrix", this->transformMatrix);
    this->setupLinesVAO();

    //Initialize Basic Texture Shaders for debug textures
    this->basicTextureShader = Shader((ASSETS_PREFIX+"shaders/basicTexture.vert").c_str(), (ASSETS_PREFIX+"shaders/basicTexture.frag").c_str());
    this->basicTextureShader.use();
    this->basicTextureShader.setMat4("uTransformMatrix", this->transformMatrix);
    this->basicTextureShader.setTextures("uTextures", samplersCount, samplers);

    //Initialize Tile Shaders, VAO, and VBO
    this->tileShader = Shader((ASSETS_PREFIX+"shaders/tiles.vert").c_str(), (ASSETS_PREFIX+"shaders/tiles.frag").c_str());
    this->tileShader.use();
    this->tileShader.setMat4("uTransformMatrix", this->transformMatrix);
    this->tileShader.setTextures("uTextures", samplersCount, samplers);
    this->setupTilesVAO();

    //Initialize Tile Shaders, VAO, and VBO
    this->spriteShader = Shader((ASSETS_PREFIX+"shaders/sprites.vert").c_str(), (ASSETS_PREFIX+"shaders/sprites.frag").c_str());
    this->spriteShader.use();
    this->spriteShader.setMat4("uTransformMatrix", this->transformMatrix);
    this->setupSpritesVAO();

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
** VBOs and VAOs managements
*/

void Renderer::setupTilesVAO(){
    assert(this->abstractTileVAO == 0 && "ERROR: called setupAbstractTileVAO multiple times!");

    //Setup data
    float s = TILE_SIZE / 2.0f;
    float spriteSizePixels = 8.0f;
    //float sheetSize = 16.0f*spriteSizePixels;
    float sheetSize = TEXTURE_ATLAS_SIZE;
    float ssf = spriteSizePixels / sheetSize; //SpriteSizeFloat

    float abstractTileData[] = {
        //Positions   //TexCoord
        -s,  s,       0.0f, 0.0f,
         s, -s,        ssf,  ssf,
        -s, -s,       0.0f,  ssf,
        -s,  s,       0.0f, 0.0f,
         s, -s,        ssf,  ssf,
         s,  s,        ssf, 0.0f,

    };

    std::cout << "ssf " << ssf << std::endl;

    //Create and bind the VAO and VBO for instanced abstract tiles data
    glGenVertexArrays(1, &this->abstractTileVAO);
    glGenBuffers(1, &this->abstractTileVBO);
    glBindVertexArray(this->abstractTileVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->abstractTileVBO);

    //Set the VBO data
    glBufferData(GL_ARRAY_BUFFER, sizeof(abstractTileData), abstractTileData, GL_STATIC_DRAW);

    //Enable 0th input as 2 floats
    glBindBuffer(GL_ARRAY_BUFFER, this->abstractTileVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    //Enable 1st input as 2 floats
    glBindBuffer(GL_ARRAY_BUFFER, this->abstractTileVBO);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));

    //Create, bind, and set the buffer for individual tile data (updated later when necessary)
    const int defaultSize = 1;
    float* rawData = new float[2*2*defaultSize];
    rawData[0] = 0.0f; rawData[1] = 0.0f; rawData[2] = 0.0f; rawData[3] = 0.0f;
    glGenBuffers(1, &this->renderableTilesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->renderableTilesVBO);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(float)*2*2) * defaultSize, &rawData[0], GL_DYNAMIC_DRAW);

    //Enabled 2nd input as 2 floats
    glBindBuffer(GL_ARRAY_BUFFER, this->renderableTilesVBO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);

    //Enabled 3rd input as 2 floats
    glBindBuffer(GL_ARRAY_BUFFER, this->renderableTilesVBO);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
    glVertexAttribDivisor(3, 1);

    //Unbind buffer and set instanced vertex attrib advancement rate
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::setupSpritesVAO(){
    assert(this->spriteVAO == 0 && "ERROR: called setupSpritesVAO multiple times!");

    //Create and bind the VAO and VBO for instanced abstract tiles data
    glGenVertexArrays(1, &this->spriteVAO);
    glGenBuffers(1, &this->spriteVBO);
    glBindVertexArray(this->spriteVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->spriteVBO);

    //Enable 0th input as 2 floats
    glBindBuffer(GL_ARRAY_BUFFER, this->spriteVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    //Unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::setupLinesVAO(){
    assert(this->linesVAO == 0 && "ERROR: called setupLinesVAO multiple times!");

    //Create and bind the VAO and VBO for instanced abstract tiles data
    glGenVertexArrays(1, &this->linesVAO);
    glGenBuffers(1, &this->linesVBO);
    glBindVertexArray(this->linesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->linesVBO);

    //Enable 0th input as 2 floats (position)
    glBindBuffer(GL_ARRAY_BUFFER, this->linesVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    //Enable 1th input as 3 floats (color)
    glBindBuffer(GL_ARRAY_BUFFER, this->linesVBO);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2*sizeof(float)));

    //Unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::updateRenderableTilesVBO(std::vector<TileRenderData>& tilesData){
    //NOTE: could be better to create a larger buffer and only call glBufferSubData instead of calling glBufferData to resize it
    if (tilesData.size() == 0){return;}

    //Update the count of how many tiles to render
    this->tilesToRender = tilesData.size();
    float* rawData = new float[2*2*this->tilesToRender];
    //float textureUnit = 8.0f/(7.0f*8.0f);
    float textureUnit = 8.0f/TEXTURE_ATLAS_SIZE;
    int index = 0;
    for (auto &td : tilesData) {
        rawData[index] = td.pos.x; index++;
        rawData[index] = td.pos.y; index++;
        rawData[index] = td.textureAtlasOffset.x + td.spriteOffset.x * textureUnit; index++;
        rawData[index] = td.textureAtlasOffset.y + td.spriteOffset.y * textureUnit; index++;
    }

    //Bind the buffer and update it's data by creating a new one and make the old one an orphan
    glBindBuffer(GL_ARRAY_BUFFER, this->renderableTilesVBO);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(float)*2*2) * this->tilesToRender, &rawData[0], GL_DYNAMIC_DRAW);

    //Clear the temp data and unbind the buffer
    delete[] rawData;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::updateRenderableSpritesVBO(std::vector<SpriteRenderData>& spritesData){
    if (spritesData.size() == 0){return;}

    //Update the count of how many tiles to render
    this->spritesToRender = spritesData.size();
    glm::vec2* rawData = new glm::vec2[6*spritesToRender];
    int index = 0;
    for (auto &sd : spritesData) {
        float w2 = sd.size.x/2.0f;
        float h2 = sd.size.y/2.0f;
        float sp = sd.spriteIndex;

        rawData[index] = {-w2+sd.pos.x,  h2+sd.pos.y}; index++;
        rawData[index] = { w2+sd.pos.x, -h2+sd.pos.y}; index++;
        rawData[index] = {-w2+sd.pos.x, -h2+sd.pos.y}; index++;
        rawData[index] = {-w2+sd.pos.x,  h2+sd.pos.y}; index++;
        rawData[index] = { w2+sd.pos.x, -h2+sd.pos.y}; index++;
        rawData[index] = { w2+sd.pos.x,  h2+sd.pos.y}; index++;
    }


    //Bind the buffer and update it's data by creating a new one and make the old one an orphan
    glBindBuffer(GL_ARRAY_BUFFER, this->spriteVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 6 * this->spritesToRender, &rawData[0], GL_DYNAMIC_DRAW);

    //Clear the temp data and unbind the buffer
    delete[] rawData;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::addLineToRender(const fPoint& p1, const fPoint& p2, SDL_Color c){
    LineRenderData rd;
    rd.p1.x = p1.x; rd.p1.y = p1.y;
    rd.p2.x = p2.x; rd.p2.y = p2.y;
    rd.col.r = c.r; rd.col.g = c.g; rd.col.b = c.b;
    this->linesRenderData.push_back(rd);
}

void Renderer::addRectToRenderP1P2(const fPoint& p1, const fPoint& p2, SDL_Color c){
    this->addLineToRender({p1.x, p1.y}, {p2.x, p1.y}, c);
    this->addLineToRender({p2.x, p1.y}, {p2.x, p2.y}, c);
    this->addLineToRender({p2.x, p2.y}, {p1.x, p2.y}, c);
    this->addLineToRender({p1.x, p2.y}, {p1.x, p1.y}, c);
}

void Renderer::addRectToRenderCentered(const fPoint& center, const fSize& size, SDL_Color c){
    float w2 = size.w/2.0f;float h2 = size.h/2.0f;
    this->addLineToRender({center.x-w2, center.y-h2}, {center.x+w2, center.y-h2}, c);
    this->addLineToRender({center.x+w2, center.y-h2}, {center.x+w2, center.y+h2}, c);
    this->addLineToRender({center.x+w2, center.y+h2}, {center.x-w2, center.y+h2}, c);
    this->addLineToRender({center.x-w2, center.y+h2}, {center.x-w2, center.y-h2}, c);
}

void Renderer::_updateLinesVBO(){
    if (linesRenderData.size() == 0){return;}

    //Update the count of how many tiles to render
    this->linesToRender = linesRenderData.size();
    float* rawData = new float[2*5*linesToRender];
    int index = 0;
    for (auto &rd : linesRenderData) {
        rawData[index] = rd.p1.x; index++;
        rawData[index] = rd.p1.y; index++;
        rawData[index] = rd.col.r/255.0f; index++;
        rawData[index] = rd.col.g/255.0f; index++;
        rawData[index] = rd.col.b/255.0f; index++;

        rawData[index] = rd.p2.x; index++;
        rawData[index] = rd.p2.y; index++;
        rawData[index] = rd.col.r/255.0f; index++;
        rawData[index] = rd.col.g/255.0f; index++;
        rawData[index] = rd.col.b/255.0f; index++;
    }


    //Bind the buffer and update it's data by creating a new one and make the old one an orphan
    glBindBuffer(GL_ARRAY_BUFFER, this->linesVBO);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 5) * 2 * this->linesToRender, &rawData[0], GL_DYNAMIC_DRAW);

    //Clear the temp data and unbind the buffer
    delete[] rawData;
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Clean the linesRenderData for the next frame
    linesRenderData.clear();
}

glm::uint Renderer::_loadTextureFromFile(const std::string& path){
    std::cout << "Loading texture from file " << path << std::endl;
    int w, h, bits;
    auto* pixels = stbi_load(path.c_str(), &w, &h, &bits, STBI_rgb_alpha);
    glm::uint textureId; glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    stbi_image_free(pixels);
    std::cout << "Loaded at id " << textureId << std::endl;
    return textureId;
}

glm::uint Renderer::_loadTextureFromSurface(SDL_Surface* s){
    int w = s->w; int h = s->h; int bits = s->format->BitsPerPixel;
    std::cout << "Loading texture from surface " << (void*)s << " of ["<<w<<","<<h<<"] and "<<bits<< " bits" << std::endl;
    auto* pixels = s->pixels;
    glm::uint textureId; glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    std::cout << "Loaded at id " << textureId << std::endl;
    return textureId;
}


void Renderer::_updateTextureFromSurface(glm::uint textureId, SDL_Surface* s){
    int w = s->w; int h = s->h; int bits = s->format->BitsPerPixel;
    auto* pixels = s->pixels;
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
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

    //Tiles draw call
    tileShader.use();

    glActiveTexture(GL_TEXTURE0);
    //glBindTextureUnit(0, this->tilesTextureId);
    glBindTexture(GL_TEXTURE_2D, this->tilesTextureId);

    tileShader.setMat4("uCameraMatrix", s->getCamera().getCameraMatrix());
    glBindVertexArray(this->abstractTileVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, this->tilesToRender);
    glBindVertexArray(0);

    //Sprites draw call
    spriteShader.use();
    spriteShader.setMat4("uCameraMatrix", s->getCamera().getCameraMatrix());
    glBindVertexArray(this->spriteVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6 * this->spritesToRender);
    glBindVertexArray(0);

    //Lines draw call
    this->_updateLinesVBO();
    basicShader.use();
    basicShader.setMat4("uCameraMatrix", s->getCamera().getCameraMatrix());
    glBindVertexArray(this->linesVAO);
    glDrawArrays(GL_LINES, 0, 2 * this->linesToRender);
    glBindVertexArray(0);


    //Noise Generator debug
    if (debugTextureFinal.initialized && false){
        basicTextureShader.use();

        basicTextureShader.setMat4("uCameraMatrix", glm::mat4(1.0f));
        debugTextureFinal.renderUnoptimized(debugTextureFinal.w/2.0f+10.0f, screenRes.h - (debugTextureFinal.h/2.0f+10.0f));
        debugTextureContinentalness.renderUnoptimized(debugTextureContinentalness.w/2.0f+10.0f, debugTextureContinentalness.h/2.0f+10.0f + debugTextureFinal.h + 10.0f);
        debugTextureErosion.renderUnoptimized(debugTextureErosion.w/2.0f+10.0f, debugTextureErosion.h/2.0f+10.0f + debugTextureFinal.h + 10.0f + debugTextureContinentalness.h + 10.0f);
    }
}

void Renderer::renderGUI(Scene* s){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    //Scene GUI
    s->renderGUI();

    //Noise Generator debug
    if (gen.renderGUI() || debugTextureFinal.initialized == false){
        DebugSurfaces ds;
        gen.generateTerrainInstanceSettings_test({1200,400}, 3, &ds);
        debugTextureFinal = TextureGL(ds.finalSurface);
        debugTextureContinentalness = TextureGL(ds.contSurface);
        debugTextureErosion = TextureGL(ds.erosionSurface);
        ds.free();
        std::cout << "pippozzo" << std::endl;
    }

    ImGui::Begin("Renderer");
    ImGui::Text("Renderer");
    ImGui::Text("Avg Fps: %f", global_avgFPS);
    ImGui::Separator();
    ImGui::Text("Tiles count: %d", tilesToRender);
    ImGui::Text("Sprites count: %d", spritesToRender);
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





    //SDL_RenderCopyEx(sdlRenderer, t.sdlTexture, NULL, &dstRect, angleDeg, NULL, SDL_FLIP_NONE);
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
