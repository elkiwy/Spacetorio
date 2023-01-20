#ifndef TEXTURE_H_
#define TEXTURE_H_


#include "SDL.h"
#include <iostream>


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

#endif // TEXTURE_H_
