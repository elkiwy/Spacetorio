#include "Entity_tile.hpp"

#include "Components.hpp"
#include "Components_colliders.hpp"
#include "SceneBiome.hpp"

/*
** Tile Entity
*/

TileEntity::TileEntity(SceneBiome* s, fPoint pos) {
    this->scene = static_cast<Scene*>(s);
    this->enttHandle = this->scene->newEntity();
    addComponent<TagComponent>("Tile");
    auto& tileComp = addComponent<TileComponent>(s, this->enttHandle);
    auto& posComp = addComponent<StaticPositionComponent>(pos);
    auto& renderableRect = addComponent<RenderableTileComponent>(1);
    auto& collider = addComponent<ColliderRectangleComponent>(fSize(TILE_SIZE, TILE_SIZE), &posComp);
}



/*
** Tile Component
*/

TileComponent::TileComponent(SceneBiome* s, entt::entity e){
    this->scene = s;
    this->enttHandle = e;
}

void TileComponent::updateSprite(){
    //TODO
}
