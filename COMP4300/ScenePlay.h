#pragma once

#include "Components.h"
#include "EntityManager.h"
#include "Scene.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include <random>
#include <tuple>

struct PlayerConfig { uint32_t SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { uint32_t SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { uint32_t SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };


class ScenePlay : public goldenhand::Scene
{
public:
    typedef std::tuple<CTransform, CShape, CBoundingBox, CScore, CLifeSpan> ComponentTuple;

    enum class ShooterEntityTag
    {
        Player,
        Enemy,
        SmallEnemy,
        Bullet,
    };

    ScenePlay(goldenhand::GameEngine& engine);

    virtual void initialize() override;

    virtual void update() override;
    virtual void sDoAction(const goldenhand::Action action) override;
    virtual void sRender() override;

private:
    sf::Font mFont;
    sf::Text mText;
    PlayerConfig mPlayerCfg;
    EnemyConfig mEnemyCfg;
    BulletConfig mBulletCfg;
    int mScore = 0;
    int mLastEnemySpawnTime = 0;
    bool mDrawBB;

    std::random_device dev;
    std::mt19937 rng;
    std::unique_ptr<std::uniform_int_distribution<>> nextEnemyPosX;
    std::unique_ptr<std::uniform_int_distribution<>> nextEnemyPosY;
    std::unique_ptr<std::uniform_int_distribution<>> nextEnemyVCount;
    std::unique_ptr<std::uniform_int_distribution<short>> nextColorComp;
    std::unique_ptr<std::uniform_real_distribution<>> nextEnemySpeed;
    std::unique_ptr<std::uniform_real_distribution<>> nextAngle;

    typedef goldenhand::EntityManager<ShooterEntityTag, ComponentTuple> EntityManager;
    typedef EntityManager::Entity Entity;
    EntityManager mEntityManager;
    std::shared_ptr<Entity> mPlayer;

    void sMovement();
    void sLifespan();
    void sEnemySpawner();
    void sCollision();

    bool collide(const std::shared_ptr<Entity>& one, const std::shared_ptr<Entity>& other) const;
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(const std::shared_ptr<Entity>& enemy);
    void spawnBullet(const std::shared_ptr<Entity>& entity, const sf::Vector2i& mousePos);
};
