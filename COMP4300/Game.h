#pragma once

#include "EntityManager.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
public:
    Game() = default;
    void init(const std::string& config);
    void run();



private:
    sf::RenderWindow mRenderWindow;
    EntityManager mEntityManager;
    sf::Font mFont;
    sf::Text mText;
    PlayerConfig mPlayerConfig;
    EnemyConfig mEnemyConfig;
    BulletConfig mBulletConfig; 
    int mScore;
    int mCurrentFrame;
    int mLastEnemySpawnTime;
    bool mPaused;
    bool mRunning;

    std::shared_ptr<Entity> mPlayer;
    void pause();

    void sMovement();
    void sUserInput();
    void sLifespan();
    void sEnemySpawner();
    void sCollision();

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> enemy);
    void spawnBullet(std::shared_ptr<Entity> entity, const sf::Vector2i& mousePos);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

};

