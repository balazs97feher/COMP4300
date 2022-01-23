#pragma once

#include "AssetManager.h"
#include "Components.h"
#include "EntityManager.h"
#include "Physics.h"
#include "Scene.h"

class ScenePlatform : public goldenhand::Scene
{
public:
    typedef std::tuple<CTransform, CBoundingBox, CAnimation> ComponentTuple;

    enum class EntityTag
    {
        Player,
        Decoration,
        Tile
    };

    struct PlayerConfig { float speed; };

    ScenePlatform(goldenhand::GameEngine& engine);

    virtual void initialize() override;

    virtual void update() override;
    virtual void sDoAction(const goldenhand::Action action) override;
    virtual void sRender() override;

private:
    goldenhand::AssetManager mAssetManager;

    typedef goldenhand::EntityManager<EntityTag, ComponentTuple> EntityManager;
    typedef EntityManager::Entity Entity;
    EntityManager mEntityManager;

    PlayerConfig mPlayerConfig;
    std::shared_ptr<Entity> mPlayer;

    goldenhand::Physics mPhysics;

    void sMovement();
    void sPhysics();
    void sAnimation();
    void spawnPlayer();
};
