#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_


#include "Utils_data.hpp"

#include "Texture.hpp"
#include "glm/glm.hpp"
#include <map>
#include <vector>




struct TextureRefInAtlas{
    glm::uint textureIndex = 0;
    float offX = 0; float offY = 0;
    glm::uint sizeW = 0; glm::uint sizeH = 0;
};


class TextureManager{
    public:
        TextureManager() {}
        ~TextureManager() {}

        void init();
        void addImage(const std::string& path);
        SDL_Surface* createTextureAtlasSurface();

        const TextureRefInAtlas& getInfoAbout(const std::string& originalPath);

    private:
        std::vector<std::string> imagesToLoad;
        std::map<std::string, TextureRefInAtlas> textureNameToRefInAtlas;
};


#endif // TEXTUREMANAGER_H_
