#include "Entity_drop.hpp"

#include "Components.hpp"
#include "Components_colliders.hpp"
#include "Components_clickables.hpp"
#include "SceneBiome.hpp"


/*
** Drop Entity
*/

DropEntity::DropEntity(SceneBiome* s, fPoint pos) {
    this->scene = static_cast<Scene*>(s);
    this->enttHandle = this->scene->newEntity();
    addComponent<TagComponent>("Drop");

    //Attach drop component
    auto& tileComp = addComponent<DropComponent>(s, this->enttHandle);

    //Attach position component
    auto& posComp = addComponent<PositionComponent>(pos);
    posComp.spd.x = randFloat(-1.5f, 1.5f);

    //Attach renderable rectangle
    auto& renderableRect = addComponent<RenderableSpriteComponent>(1, fSize(8.0f, 8.0f));
}



/*
** Drop Component
*/

DropComponent::DropComponent(SceneBiome* s, entt::entity e){
    this->scene = s;
    this->enttHandle = e;
}

void DropComponent::update(Entity& e, const Uint8* ks){
    //Gravity update
    auto& posComp = e.getComponent<PositionComponent>();
    posComp.spd -= this->scene->BIOME_GRAVITY;
    posComp.spd.y = fmin(this->scene->BIOME_TERMVELOCITY, posComp.spd.y);

    //Update the colliders to be in the position + speed location
    fPoint simulatedNextPos = posComp.pos + posComp.spd;
    collider.c.x = simulatedNextPos.x;
    collider.c.y = simulatedNextPos.y;

    //Get the tiles near the collider and check the collisions
    ShapeRectangle testerShape = collider.toRectangle();
    std::vector<TileBiome*> tiles = this->scene->getTilesInRect(testerShape);
    for(auto& tile : tiles){
        if (tile->isEmpty()){continue;}
        //Check collision with each entity in the tile
        for(auto& ent : tile->entities){
            Entity e = {ent, scene};
            if (e.hasComponent<ColliderComponent>() == false){continue;}
            auto& genericCollider = e.getComponent<ColliderComponent>();
            for(auto impl: genericCollider.impls){
                if (impl == nullptr){break;}
                const Shape& tileShape = static_cast<ColliderComponent*>(impl)->getShape();
                if (testerShape.checkCollision(tileShape)){
                    //Collision!
                    posComp.spd.x = 0.0f;
                    posComp.spd.y = 0.0f;
                }
            }
        }
    }

    //Update position with speed
    posComp.pos += posComp.spd;
}
