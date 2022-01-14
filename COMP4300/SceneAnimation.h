#pragma once

#include "AssetManager.h"
#include "Scene.h"

class SceneAnimation : public Scene
{
public:
    SceneAnimation(GameEngine& engine);

    virtual void initialize() override;

    virtual void update() override;
    virtual void sDoAction(const Action action) override;
    virtual void sRender() override;

private:
    AssetManager mAssetManager;
    EntityManager mEntityManager;

    std::shared_ptr<Entity> mPlayer;

    void sMovement();
    void sAnimation();
    void spawnPlayer();
};

