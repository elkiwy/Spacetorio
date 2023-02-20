#include "TextureManager.hpp"
#include "SDL_surface.h"
#include "SDL_image.h"
#include "Utils_data.hpp"

void TextureManager::init(){

}

void TextureManager::addImage(const std::string& path){
    if (std::find(imagesToLoad.begin(), imagesToLoad.end(), path) == imagesToLoad.end()) {
        imagesToLoad.push_back(path);
    }
}


SDL_Surface* TextureManager::createTextureAtlasSurface(){
    //Create the new empty atlas
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    const uint32_t R_MASK = 0xff000000;
    const uint32_t G_MASK = 0x00ff0000;
    const uint32_t B_MASK = 0x0000ff00;
    const uint32_t A_MASK = 0x000000ff;
    #else
    const uint32_t R_MASK = 0x000000ff;
    const uint32_t G_MASK = 0x0000ff00;
    const uint32_t B_MASK = 0x00ff0000;
    const uint32_t A_MASK = 0xff000000;
    #endif
    SDL_Surface* textureAtlas = SDL_CreateRGBSurface(0, TEXTURE_ATLAS_SIZE, TEXTURE_ATLAS_SIZE, 32, R_MASK, G_MASK, B_MASK, A_MASK);

    //Cycle through each image on the list and blit it to the texture atlas
    int offsetX = 0;
    for (auto& path : imagesToLoad){
        //Load image and blit it to the texture atlas
        SDL_Surface* img = IMG_Load(path.c_str());
        SDL_Rect dstRect = {offsetX, 0, img->w, img->h};
        SDL_BlitSurface(img, NULL, textureAtlas, &dstRect);

        //Save his referece data into the map
        TextureRefInAtlas ref;
        ref.textureIndex = 0;
        ref.offX  = ((float)dstRect.x)/TEXTURE_ATLAS_SIZE;
        ref.offY  = ((float)dstRect.y)/TEXTURE_ATLAS_SIZE;
        ref.sizeW = dstRect.w;
        ref.sizeH = dstRect.h;
        textureNameToRefInAtlas[path] = ref;

        //Update the offset for the next ones
        offsetX += dstRect.w;
    }

    //Clear the image list
    imagesToLoad.clear();

    //Debug save atlas
    SDL_SaveBMP(textureAtlas, "TextureAtlas.bmp");

    return textureAtlas;
}


const TextureRefInAtlas& TextureManager::getInfoAbout(const std::string& originalPath){
    return textureNameToRefInAtlas[originalPath];
}
