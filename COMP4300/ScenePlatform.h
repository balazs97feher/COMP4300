#pragma once

#include "AssetManager.h"
#include "Components.h"
#include "EntityManager.h"
#include "Physics.h"
#include "Scene.h"

class ScenePlatform : public goldenhand::Scene
{
public:
    typedef std::tuple<CTransform, CBoundingBox, CAnimation, CDraggable, CLifeSpan, CGravity> ComponentTuple;

    enum class EntityTag
    {
        Player,
        Background,
        Tile,
        Blade
    };

    struct PlayerConfig { float startPosX, startPosY, runSpeed, jumpSpeed, maxSpeed, trapViewRatio; };
    struct BulletConfig { float speed, rotation, lifespan; };

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
    BulletConfig mBulletConfig;
    std::shared_ptr<Entity> mPlayer;

    goldenhand::Physics mPhysics;
    bool mDrawBB;
    bool mTextureDraw;
    sf::RectangleShape mBB;

    void sView();
    void sMovement();
    void sPhysics();
    void sAnimation();
    void sLifeSpan();
    void spawnPlayer();
    void shootBlade();

    const char* mLevel = "./config/level1.txt";
    void saveLevel();

    bool mCloneSelected;
    std::shared_ptr<Entity> mDraggedEntity;
    std::shared_ptr<Entity> findSelectedEntity(const sf::Vector2f spot);
};
