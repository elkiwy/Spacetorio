#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include "Texture.hpp"
#include "glm/glm.hpp"
#include <map>
#include <vector>


#define TEXTURE_ATLAS_SIZE 2048


struct TextureRefInAtlas{
    glm::uint textureIndex = 0;
    glm::uint  offX = 0; glm::uint  offY = 0;
    glm::uint sizeW = 0; glm::uint sizeH = 0;
};


class TextureManager{
    public:
        TextureManager() {}
        ~TextureManager() {}

        void init();
        void addImage(const std::string& path);
        SDL_Surface* createTextureAtlasSurface();

    private:
        std::vector<std::string> imagesToLoad;
        std::map<std::string, TextureRefInAtlas> textureNameToRefInAtlas;
};


#endif // TEXTUREMANAGER_H_
