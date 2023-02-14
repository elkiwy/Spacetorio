#include "Entity_tile.hpp"

#include "Components.hpp"
#include "Components_colliders.hpp"
#include "Components_clickables.hpp"
#include "SceneBiome.hpp"
#include <cmath>

/*
** Tile Entity
*/

TileEntity::TileEntity(SceneBiome* s, fPoint pos, MaterialType matType) {
    this->scene = static_cast<Scene*>(s);
    this->enttHandle = this->scene->newEntity();
    addComponent<TagComponent>("Tile");
    auto& tileComp = addComponent<TileComponent>(s, this->enttHandle, matType);
    auto& posComp = addComponent<StaticPositionComponent>(pos);

    const TextureRefInAtlas& textureRef = global_renderer->getTextureManager().getInfoAbout(tileComp.material.spritePath);
    auto& renderableRect = addComponent<RenderableTileComponent>(textureRef.offX, textureRef.offY);

    auto& collider = addComponent<ColliderRectangleComponent>(fSize(TILE_SIZE, TILE_SIZE), &posComp);

    auto& clickable = addComponent<ClickableRectangleComponent>(fSize(TILE_SIZE, TILE_SIZE), &posComp);
    clickable.onleftmousedown = [&tileComp](){tileComp.onLeftMouseDown();};
}



/*
** Tile Component
*/

TileComponent::TileComponent(SceneBiome* s, entt::entity e, MaterialType matType){
    this->scene = s;
    this->enttHandle = e;
    this->material = getMaterialDataForType(matType);
    this->hp = material.hp;
}

void TileComponent::onLeftMouseDown(){
    this->hp -= 0.5f;
    if (this->hp <= 0.0f){destroyTile();}
}

void TileComponent::destroyTile(){
    this->scene->removeTile(this->enttHandle);
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
