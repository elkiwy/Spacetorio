#ifndef TEXTURE_H_
#define TEXTURE_H_

//OpenGL Stuff
#include "GL/glew.h"

//STB Stuff
#include "SDL_surface.h"
#include "stb_image.h"

#include "SDL.h"
#include <iostream>
#include "glm/glm.hpp"



class Renderer;
extern Renderer* global_renderer;

struct Texture{
    bool initialized = false;
    SDL_Texture* sdlTexture = nullptr;
    size_t w = 0;
    size_t h = 0;

    Texture() = default;
    Texture(SDL_Surface *s, bool blended = true);

    void free(){SDL_DestroyTexture(this->sdlTexture); w = 0; h = 0; initialized = false;}

};

struct TextureGL{
    bool initialized = false;
    int w = 0; int h = 0;
    glm::uint textureId = 0;
    glm::uint vao = 0;
    glm::uint vbo = 0;

    TextureGL(){}

    TextureGL(SDL_Surface* s){
        this->w = s->w; this->h = s->h;
        auto* pixels = s->pixels;
        glGenTextures(1, &this->textureId);
        glBindTexture(GL_TEXTURE_2D, this->textureId);
        this->setupParams();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        this->setupVAO();
        this->initialized = true;
    }

    TextureGL(const std::string& path){
        int bits;
        auto* pixels = stbi_load(path.c_str(), &this->w, &this->h, &bits, STBI_rgb_alpha);
        glGenTextures(1, &this->textureId);
        glBindTexture(GL_TEXTURE_2D, this->textureId);
        this->setupParams();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_image_free(pixels);
        this->setupVAO();
        initialized = true;
    }

    void updateData(SDL_Surface* s){
        this->w = s->w; this->h = s->h;
        auto* pixels = s->pixels;
        glBindTexture(GL_TEXTURE_2D, this->textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }

    void setupParams(){
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    void setupVAO(){
        //Create and bind the VAO and VBO for instanced abstract tiles data
        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &this->vbo);
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

        //Enable 0th input as 2 floats
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        //Enable 0th input as 2 floats
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));

        //Unbind buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }


    void renderUnoptimized(float x, float y){
        //Update the vbo before drawing
        float w2 = ((float)w)/2.0f;
        float h2 = ((float)h)/2.0f;
        glm::vec4* rawData = new glm::vec4[6];
        rawData[0] = {-w2+x,  h2+y, 0.0f, 0.0f};
        rawData[1] = { w2+x, -h2+y, 1.0f, 1.0f};
        rawData[2] = {-w2+x, -h2+y, 0.0f, 1.0f};
        rawData[3] = {-w2+x,  h2+y, 0.0f, 0.0f};
        rawData[4] = { w2+x, -h2+y, 1.0f, 1.0f};
        rawData[5] = { w2+x,  h2+y, 1.0f, 0.0f};
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 6, &rawData[0], GL_DYNAMIC_DRAW);
        delete[] rawData;
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Draw call
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->textureId);
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void free(){
        //TODO
    }

};

#endif // TEXTURE_H_
