#include "Entity_tile.hpp"

#include "Components.hpp"
#include "Components_colliders.hpp"
#include "Components_clickables.hpp"
#include "SceneBiome.hpp"
#include <cmath>

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
    auto& clickable = addComponent<ClickableRectangleComponent>(fSize(TILE_SIZE, TILE_SIZE), &posComp);

    clickable.onclick = [](){
        std::cout << "Ciaoooo" << std::endl;
    };

    entt::entity e = this->enttHandle;
    clickable.onleftmousedown = [e](){
        std::cout << "Click down! " << (int)e << std::endl;
    };

}



/*
** Tile Component
*/

TileComponent::TileComponent(SceneBiome* s, entt::entity e){
    this->scene = s;
    this->enttHandle = e;
}

void TileComponent::updateSprite(){
    Entity e = {this->enttHandle, this->scene};
    auto& renderableTile = e.getComponent<RenderableTileComponent>();
    auto& pos = e.getComponent<StaticPositionComponent>();
    int surroundingVal = this->scene->getTileSurroundingValue((int)floor(pos.pos.x/TILE_SIZE), (int)floor(pos.pos.y/TILE_SIZE));
    float offx, offy;
    getSpriteOffsetFromTileSurroundingValue(surroundingVal, &offx, &offy);
    renderableTile.renderData.spriteOffset.x = offx;
    renderableTile.renderData.spriteOffset.y = offy;
}
