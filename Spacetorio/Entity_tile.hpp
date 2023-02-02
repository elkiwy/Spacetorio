#ifndef ENTITY_TILE_H_
#define ENTITY_TILE_H_

#include "Data_materials.hpp"
#include "Utils_points.hpp"
#include "Components_position.hpp"
#include "Components_updatable.hpp"
#include "Components_renderables.hpp"
#include "Entity.hpp"
#include "Utils_data.hpp"

class SceneBiome;

class TileEntity : public Entity{
    public:
        TileEntity() {}
        TileEntity(const TileEntity &p) = default;
        TileEntity(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        TileEntity(SceneBiome *s, fPoint pos);
        virtual ~TileEntity() {}
};



struct TileComponent{
    fSize size = {TILE_SIZE, TILE_SIZE};
    SceneBiome* scene = nullptr;
    entt::entity enttHandle;
    MaterialData material;

    TileComponent() = default;
    TileComponent(const TileComponent&) = default;
    TileComponent(SceneBiome* s, entt::entity e);
    ~TileComponent(){};

    void updateSprite();
};




#endif // ENTITY_TILE_H_
