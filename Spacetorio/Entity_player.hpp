#ifndef ENTITY_PLAYER_H_
#define ENTITY_PLAYER_H_

#include "Utils_points.hpp"
#include "Components_position.hpp"
#include "Components_updatable.hpp"
#include "Components_renderables.hpp"
#include "Entity.hpp"
#include "Utils_data.hpp"

class SceneBiome;

class PlayerEntity : public Entity{
    public:
        PlayerEntity() {}
        PlayerEntity(const PlayerEntity &p) = default;
        PlayerEntity(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        PlayerEntity(SceneBiome *s, fPoint pos);
        virtual ~PlayerEntity() {}
};



struct PlayerComponent : public UpdatableComponent{
    fSize size = {TILE_SIZE*1.1f, TILE_SIZE*2.0f};
    ShapeLine groundCollider;
    ShapeLine wallLCollider;
    ShapeLine wallRCollider;
    SceneBiome* scene = nullptr;
    entt::entity enttHandle;

    PlayerComponent() = default;
    PlayerComponent(const PlayerComponent&) = default;
    PlayerComponent(SceneBiome* s, entt::entity e);
    ~PlayerComponent(){};

    void updateColliders(fPoint wPos);

    void update(Entity& e, const Uint8* keyState) override;
    void onKeyPressed(SDL_Keycode key) override;

  private:
    bool testCollisionWithLine(const ShapeLine& line) const;
};



#endif // ENTITY_PLAYER_H_
