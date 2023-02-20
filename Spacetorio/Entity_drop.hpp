#ifndef ENTITY_DROP_H_
#define ENTITY_DROP_H_


#include "Components_updatable.hpp"
#include "Entity.hpp"
#include "Utils_data.hpp"
#include "Utils_points.hpp"
#include "Utils_geometry.hpp"

class SceneBiome;

class DropEntity : public Entity{
    public:
        DropEntity() {}
        DropEntity(const DropEntity &p) = default;
        DropEntity(entt::entity handle, Scene *s) {enttHandle = handle; scene = s;}
        DropEntity(SceneBiome *s, fPoint pos);
        virtual ~DropEntity() {}
};



struct DropComponent : public UpdatableComponent{
    SceneBiome* scene = nullptr;
    entt::entity enttHandle;
    ShapeCircle collider;

    DropComponent() = default;
    DropComponent(const DropComponent&) = default;
    DropComponent(SceneBiome* s, entt::entity e);
    ~DropComponent(){};

    void update(Entity& e, const Uint8* ks) override;
};



#endif // ENTITY_DROP_H_
