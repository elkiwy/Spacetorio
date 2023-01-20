#include "Texture.hpp"
#include "Renderer.hpp"
#include "SDL_blendmode.h"
#include "SDL_render.h"
#include <iostream>


Texture::Texture(SDL_Surface *s, bool blended) {
    SDL_Renderer* sdlRenderer = global_renderer->getSdlRenderer();
    this->sdlTexture = SDL_CreateTextureFromSurface(sdlRenderer, s);
    this->w = s->w;
    this->h = s->h;
    this->initialized = true;
    if (blended){SDL_SetTextureBlendMode(this->sdlTexture, SDL_BLENDMODE_ADD);}
    std::cout << "Creating texture with size: " << w << " " << h << std::endl;
}
