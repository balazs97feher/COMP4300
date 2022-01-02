#pragma once

#include "EntityManager.h"

#include <random>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

struct PlayerConfig { uint32_t SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { uint32_t SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { uint32_t SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
public:
    Game() = default;
    void init(const std::string& config);
    void run();



private:
    sf::RenderWindow mRenderWindow;
    sf::Vector2u mWindowSize;
    EntityManager mEntityManager;
    sf::Font mFont;
    sf::Text mText;
    PlayerConfig mPlayerCfg;
    EnemyConfig mEnemyCfg;
    BulletConfig mBulletCfg; 
    int mScore = 0;
    int mCurrentFrame = 0;
    int mLastEnemySpawnTime = 0;
    bool mPaused = false;
    bool mRunning = true;

    std::random_device dev;
    std::mt19937 rng;
    std::unique_ptr<std::uniform_int_distribution<>> nextEnemyPosX;
    std::unique_ptr<std::uniform_int_distribution<>> nextEnemyPosY;
    std::unique_ptr<std::uniform_int_distribution<>> nextEnemyVCount;
    std::unique_ptr<std::uniform_int_distribution<short>> nextColorComp;
    std::unique_ptr<std::uniform_real_distribution<>> nextEnemySpeed;
    std::unique_ptr<std::uniform_real_distribution<>> nextAngle;

    std::shared_ptr<Entity> mPlayer;
    void pause();

    void sMovement();
    void sUserInput();
    void sLifespan();
    void sAttack();
    void sEnemySpawner();
    void sCollision();
    void sRender();

    bool collide(const std::shared_ptr<Entity>& one, const std::shared_ptr<Entity>& other) const;
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(const std::shared_ptr<Entity>& enemy);
    void spawnBullet(const std::shared_ptr<Entity>& entity, const sf::Vector2i& mousePos);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

};

