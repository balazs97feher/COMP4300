#pragma once

#include "AssetManager.h"
#include "Components.h"
#include "EntityManager.h"
#include "Scene.h"


class SceneAnimation : public goldenhand::Scene
{
public:
    typedef std::tuple<CTransform, CBoundingBox, CAnimation> ComponentTuple;

    enum class AnimationEntityTag
    {
        Player
    };

    SceneAnimation(goldenhand::GameEngine& engine);

    virtual void initialize() override;

    virtual void update() override;
    virtual void sDoAction(const goldenhand::Action action) override;
    virtual void sRender() override;

private:
    goldenhand::AssetManager mAssetManager;

    typedef goldenhand::EntityManager<AnimationEntityTag, ComponentTuple> EntityManager;
    typedef EntityManager::Entity Entity;
    EntityManager mEntityManager;

    std::shared_ptr<Entity> mPlayer;

    void sMovement();
    void sAnimation();
    void spawnPlayer();
};
