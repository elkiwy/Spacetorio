#ifndef DATA_MATERIALS_H_
#define DATA_MATERIALS_H_

#include "Utils_data.hpp"

#include <map>
#include <string>
#include <vector>

enum MaterialType {MAT_UNKOWN, MAT_DIRT, MAT_WOOD, MAT_CONCRETE,};

struct MaterialData{
    MaterialType type = MAT_UNKOWN;
    std::string spritePath = "";
    float hp = 0.0f;
};

static std::map<MaterialType, MaterialData> matTypeToMaterialData = {
    {
        MAT_UNKOWN, {
            //Type
                MAT_UNKOWN,
            //SpritePath
                ASSETS_PREFIX+"res/tileset_guide.png",
            //HP
                0.0f,
        },
    },
    {
        MAT_DIRT, {
            //Type
                MAT_DIRT,
            //SpritePath
                ASSETS_PREFIX+"res/dirt.png",
            //HP
                10.0f,
        },
    },
    {
        MAT_WOOD, {
            //Type
                MAT_WOOD,
            //SpritePath
                ASSETS_PREFIX+"res/tileset_guide.png",
            //HP
                30.0f,
        },
    },
    {
        MAT_CONCRETE, {
            //Type
                MAT_CONCRETE,
            //SpritePath
                ASSETS_PREFIX+"res/concrete.png",
            //HP
                100.0f,
        },
    },
};


inline MaterialData getMaterialDataForType(MaterialType type){
    return matTypeToMaterialData[type];
}

inline std::vector<std::string> getMaterialsSprites(){
    std::vector<std::string> sprites;
    for (auto it = matTypeToMaterialData.begin(); it != matTypeToMaterialData.end(); ++it) {
        sprites.push_back(it->second.spritePath);
    }
    return sprites;
}

#endif // DATA_MATERIALS_H_
