#pragma once

#include "Scene.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include <random>

struct PlayerConfig { uint32_t SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { uint32_t SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { uint32_t SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class ScenePlay : public Scene
{
public:
    ScenePlay(GameEngine& engine);

    virtual void initialize() override;

    virtual void update() override;
    virtual void sDoAction(const Action action) override;
    virtual void sRender() override;

private:
    sf::Font mFont;
    sf::Text mText;
    PlayerConfig mPlayerCfg;
    EnemyConfig mEnemyCfg;
    BulletConfig mBulletCfg;
    int mScore = 0;
    int mLastEnemySpawnTime = 0;

    std::random_device dev;
    std::mt19937 rng;
    std::unique_ptr<std::uniform_int_distribution<>> nextEnemyPosX;
    std::unique_ptr<std::uniform_int_distribution<>> nextEnemyPosY;
    std::unique_ptr<std::uniform_int_distribution<>> nextEnemyVCount;
    std::unique_ptr<std::uniform_int_distribution<short>> nextColorComp;
    std::unique_ptr<std::uniform_real_distribution<>> nextEnemySpeed;
    std::unique_ptr<std::uniform_real_distribution<>> nextAngle;

    std::shared_ptr<Entity> mPlayer;

    void sMovement();
    void sLifespan();
    void sAttack();
    void sEnemySpawner();
    void sCollision();

    bool collide(const std::shared_ptr<Entity>& one, const std::shared_ptr<Entity>& other) const;
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(const std::shared_ptr<Entity>& enemy);
    void spawnBullet(const std::shared_ptr<Entity>& entity, const sf::Vector2i& mousePos);

};

