#include "TextureManager.hpp"
#include "SDL_surface.h"
#include "SDL_image.h"

void TextureManager::init(){

}

void TextureManager::addImage(const std::string& path){
    imagesToLoad.push_back(path);
}

SDL_Surface* TextureManager::createTextureAtlasSurface(){
    //Create the new empty atlas
    SDL_Surface* textureAtlas = SDL_CreateRGBSurface(0, TEXTURE_ATLAS_SIZE, TEXTURE_ATLAS_SIZE, 32, 0, 0, 0, 0);

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
        ref.offX  = dstRect.x; ref.offY  = dstRect.y;
        ref.sizeW = dstRect.w; ref.sizeH = dstRect.h;
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
