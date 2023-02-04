#include "Entity_player.hpp"
#include "SceneBiome.hpp"
#include "Components.hpp"
#include "Components_colliders.hpp"


/*
** Player Entity
*/

PlayerEntity::PlayerEntity(SceneBiome* s, fPoint pos) {
    this->scene = static_cast<Scene*>(s);
    this->enttHandle = this->scene->newEntity();
    addComponent<TagComponent>("Player");

    //Attach player component
    auto& playerComp = addComponent<PlayerComponent>(s, this->enttHandle);

    //Attach position component
    auto& posComp = addComponent<DynamicPositionComponent>(pos);
    posComp.automaticUpdates = false;

    //Attach renderable rectangle
    auto& renderableRect = addComponent<RenderableSpriteComponent>(1, playerComp.size);

    //Lock camera
    s->getCamera().setTarget(&posComp);
}





/*
** Player Component
*/

PlayerComponent::PlayerComponent(SceneBiome* s, entt::entity e){
    this->scene = s;
    this->enttHandle = e;
}


void PlayerComponent::updateColliders(fPoint wPos){
    groundCollider.p1.x = wPos.x - size.w*0.5f + 1;
    groundCollider.p1.y = wPos.y - size.h*0.5f;
    groundCollider.p2.x = wPos.x + size.w*0.5f - 1;
    groundCollider.p2.y = wPos.y - size.h*0.5f;

    wallLCollider.p1.x = wPos.x - size.w*0.5f;
    wallLCollider.p1.y = wPos.y + size.h*0.5f;
    wallLCollider.p2.x = wPos.x - size.w*0.5f;
    wallLCollider.p2.y = wPos.y - size.h*0.5f + 2;

    wallRCollider.p1.x = wPos.x + size.w*0.5f;
    wallRCollider.p1.y = wPos.y + size.h*0.5f;
    wallRCollider.p2.x = wPos.x + size.w*0.5f;
    wallRCollider.p2.y = wPos.y - size.h*0.5f + 2;
}


bool PlayerComponent::testCollisionWithLine(const ShapeLine& line) const{
    //Get the tiles where the lines is
    ShapeRectangle testerShape = line.toRectangle();
    std::vector<TileBiome*> tiles = this->scene->getTilesInRect(testerShape);
    //Check collision with each tile
    for(auto& tile : tiles){
        if (tile->isEmpty()){continue;}
        //Check collision with each entity in the tile
        for(auto& ent : tile->entities){
            Entity e = {ent, scene};
            if (e.hasComponent<ColliderComponent>()){
                auto& genericCollider = e.getComponent<ColliderComponent>();
                for(auto impl: genericCollider.impls){
                    if (impl == nullptr){break;}
                    const Shape& tileShape = static_cast<ColliderComponent*>(impl)->getShape();
                    if (testerShape.checkCollision(tileShape)){
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void PlayerComponent::update(Entity& player, const Uint8* ks){
    //Position setup
    auto& posComp = player.getComponent<DynamicPositionComponent>();

    //Gravity update
    fVec gravityForce = {0.0f, 0.60f};
    float terminalVelocity = 30.0f;
    posComp.spd -= gravityForce;
    posComp.spd.y = fmin(terminalVelocity, posComp.spd.y);

    //Left+Right update
    float v = 0.0f;
    if (ks[SDL_SCANCODE_D]) v += 5.0f;
    if (ks[SDL_SCANCODE_A]) v -= 5.0f;
    posComp.spd.x = v;

    //Update the colliders to be in the position + speed location
    fPoint simulatedNextPos = posComp.pos + posComp.spd;
    this->updateColliders(simulatedNextPos);

    //Test collision to see if there is space to move with my current speed
    if (posComp.spd.x != 0 && (testCollisionWithLine(wallLCollider) || testCollisionWithLine(wallRCollider))){
        std::cout << "collision left/right" << std::endl;
        posComp.spd.x = 0;

        simulatedNextPos = posComp.pos + posComp.spd;
        this->updateColliders(simulatedNextPos);
    }

    if (testCollisionWithLine(groundCollider)){
        std::cout << "collision ground" << std::endl;
        posComp.spd.y = 0;
    }



    //Update position with speed
    posComp.pos += posComp.spd;
}

void PlayerComponent::onKeyPressed(SDL_Keycode key){
    if (key == SDLK_w) {
        Entity player = {enttHandle, scene};
        auto &posComp = player.getComponent<DynamicPositionComponent>();
        fVec jumpForce = {0.0f, 12.0f};
        posComp.spd += jumpForce;
    }
}
