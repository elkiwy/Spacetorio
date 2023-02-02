#ifndef DATA_MATERIALS_H_
#define DATA_MATERIALS_H_

#include <map>

enum MaterialType {MAT_UNKOWN, MAT_DIRT, MAT_WOOD, MAT_CONCRETE,};

struct MaterialData{
    MaterialType type = MAT_UNKOWN;
    float hp = 0.0f;
};

static std::map<MaterialType, MaterialData> matTypeToMaterialData = {
    {
        MAT_UNKOWN, {
            MAT_UNKOWN,     //Type
            0.0f,           //HP
        },
    },
    {
        MAT_DIRT, {
            MAT_DIRT,       //Type
            10.0f,          //HP
        },
    },
    {
        MAT_WOOD, {
            MAT_WOOD,       //Type
            30.0f,          //HP
        },
    },
    {
        MAT_CONCRETE, {
            MAT_CONCRETE,   //Type
            100.0f,         //HP
        },
    },
};


inline MaterialData getMaterialDataForType(MaterialType type){
    return matTypeToMaterialData[type];
}


#endif // DATA_MATERIALS_H_
