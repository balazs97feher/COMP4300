#include "Game.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numbers>

#include <SFML/Window.hpp>

namespace fs = std::filesystem;
using namespace std;

void Game::run()
{
    while (mRunning)
    {
        if (mPaused)
        {
            sRender();
            sUserInput();
        }
        else
        {
            mEntityManager.update();

            sEnemySpawner();
            sMovement();
            sCollision();
            sUserInput();

            sRender();
            mCurrentFrame++;
        }
    }
}

void Game::init(const std::string& config)
{
    const fs::path configFile{ config };
    if (!fs::exists(configFile))
    {
        cerr << "Configuration file " << configFile.string() << " does not exit." << endl;
        exit(-1);
    }

    std::ifstream configuration{ configFile };
    if (!configuration.is_open())
    {
        cerr << "Failed to open configuration file." << endl;
        exit(-1);
    }

    for (size_t i = 0; i < 5; i++)
    {
        string configType;
        configuration >> configType;
        if (configType == "Window")
        {
            int wWidth, wHeight, frameLimit, fullScreenMode;
            configuration >> wWidth >> wHeight >> frameLimit >> fullScreenMode;
            const uint32_t wStyle = (fullScreenMode == 1) ? (sf::Style::Fullscreen | sf::Style::Default) : sf::Style::Default;
            mRenderWindow.create(sf::VideoMode(wWidth, wHeight), "Shooter", wStyle);
            mRenderWindow.setFramerateLimit(frameLimit);
            mWindowSize = mRenderWindow.getSize();
        }
        else if (configType == "Font")
        {
            string font;
            int textSize, r, g, b;
            configuration >> font >> textSize >> r >> g >> b;
            mText.setCharacterSize(textSize);
            mText.setFillColor(sf::Color(r, g, b));
            if (!mFont.loadFromFile("./config/" + font))
            {
                cerr << "Failed to load font: " << font << endl;
                exit(-1);
            }
            mText.setFont(mFont);
        }
        else if (configType == "Player")
        {
            PlayerConfig pcfg;
            configuration >> pcfg.SR >> pcfg.CR >> pcfg.S >> pcfg.FR >> pcfg.FG
                >> pcfg.FB >> pcfg.OR >> pcfg.OG >> pcfg.OB >> pcfg.OT >> pcfg.V;
            mPlayerCfg = pcfg;
        }
        else if (configType == "Enemy")
        {
            EnemyConfig ecfg;
            configuration >> ecfg.SR >> ecfg.CR >> ecfg.SMIN >> ecfg.SMAX >> ecfg.OR
                >> ecfg.OG >> ecfg.OB >> ecfg.OT >> ecfg.VMIN >> ecfg.VMAX >> ecfg.L >> ecfg.SI;
            mEnemyCfg = ecfg;

            nextEnemyPosX = make_unique<std::uniform_int_distribution<>>(0, mWindowSize.x - mEnemyCfg.SR);
            nextEnemyPosY = make_unique<std::uniform_int_distribution<>>(0, mWindowSize.y - mEnemyCfg.SR);
            nextEnemyVCount = make_unique<std::uniform_int_distribution<>>(ecfg.VMIN, ecfg.VMAX);
            nextEnemySpeed = make_unique<std::uniform_real_distribution<>>(ecfg.SMIN, ecfg.SMAX);
        }
        else if (configType == "Bullet")
        {
            BulletConfig bcfg;
            configuration >> bcfg.SR >> bcfg.CR >> bcfg.S >> bcfg.FR >> bcfg.FG >> bcfg.FB
                >> bcfg.OR >> bcfg.OG >> bcfg.OB >> bcfg.OT >> bcfg.V >> bcfg.L;
            mBulletCfg = bcfg;
        }
    }

    rng = mt19937(dev());
    nextColorComp = make_unique<std::uniform_int_distribution<short>>(0, 255);
    nextAngle = make_unique<std::uniform_real_distribution<>>(0, 2 * numbers::pi);

    spawnPlayer();
}

void Game::pause()
{
}

void Game::sMovement()
{
    sf::Vector2f playerVelocity;
    if (mPlayer->cInput->left)
    {
        playerVelocity.x -= mPlayerCfg.S;
    }
    if (mPlayer->cInput->up)
    {
        playerVelocity.y -= mPlayerCfg.S;
    }
    if (mPlayer->cInput->right)
    {
        playerVelocity.x += mPlayerCfg.S;
    }
    if (mPlayer->cInput->down)
    {
        playerVelocity.y += mPlayerCfg.S;
    }
    mPlayer->cTransform->velocity = playerVelocity;

    for (auto& e : mEntityManager.getEntities())
    {
        e->cTransform->pos += e->cTransform->velocity;
    }
}

void Game::sUserInput()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) mPlayer->cInput->up = true;
    else mPlayer->cInput->up = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) mPlayer->cInput->left = true;
    else mPlayer->cInput->left = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) mPlayer->cInput->down = true;
    else mPlayer->cInput->down = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) mPlayer->cInput->right = true;
    else mPlayer->cInput->right = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) mPaused = !mPaused;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) mRunning = false;
}

void Game::sLifespan()
{
}

void Game::sEnemySpawner()
{
    if ((mCurrentFrame % mEnemyCfg.SI) == 0)
    {
        spawnEnemy();
    }
}

void Game::sCollision()
{
    mPlayer->cTransform->pos.x = clamp(mPlayer->cTransform->pos.x, (float)mPlayerCfg.SR, (float)(mWindowSize.x - mPlayerCfg.SR));
    mPlayer->cTransform->pos.y = clamp(mPlayer->cTransform->pos.y, (float)mPlayerCfg.SR, (float)(mWindowSize.y - mPlayerCfg.SR));

    for (auto& e : mEntityManager.getEntities(EntityTag::Enemy))
    {
        if ((e->cTransform->pos.x <= mEnemyCfg.SR) || (e->cTransform->pos.x + mEnemyCfg.SR >= mWindowSize.x))
        {
            e->cTransform->velocity.x *= -1;
        }
        if ((e->cTransform->pos.y <= mEnemyCfg.SR) || (e->cTransform->pos.y + mEnemyCfg.SR >= mWindowSize.y))
        {
            e->cTransform->velocity.y *= -1;
        }
    }
}

void Game::sRender()
{
    mRenderWindow.clear();
    for (auto& e : mEntityManager.getEntities())
    {
        e->cShape->circle.setPosition(e->cTransform->pos);
        mRenderWindow.draw(e->cShape->circle);
    }
    mRenderWindow.display();
}

void Game::spawnPlayer()
{
    mPlayer = mEntityManager.addEntity(EntityTag::Player);
    
    mPlayer->cTransform = make_shared<CTransform>(sf::Vector2f(mWindowSize.x / 2 - mPlayerCfg.SR, mWindowSize.y / 2 - mPlayerCfg.SR),
        sf::Vector2f{ 0, 0 }, 0.0f);

    const sf::Color fillColor(mPlayerCfg.FR, mPlayerCfg.FG, mPlayerCfg.FB);
    const sf::Color outlineColor(mPlayerCfg.OR, mPlayerCfg.OG, mPlayerCfg.OB);
    mPlayer->cShape = make_shared<CShape>(mPlayerCfg.SR, mPlayerCfg.V, fillColor,
        outlineColor, mPlayerCfg.OT);

    mPlayer->cCollision = make_shared<CCollision>(mPlayerCfg.CR);

    mPlayer->cScore = make_shared<CScore>(0);

    mPlayer->cInput = make_shared<CInput>();
}

void Game::spawnEnemy()
{
    auto enemy = mEntityManager.addEntity(EntityTag::Enemy);

    const auto posX = (*nextEnemyPosX)(rng);
    const auto posY = (*nextEnemyPosY)(rng);
    const auto speed = (*nextEnemySpeed)(rng);
    const auto vAngle = (*nextAngle)(rng);
    const auto vCount = (*nextEnemyVCount)(rng);

    enemy->cTransform = make_shared<CTransform>(sf::Vector2f(posX, posY),
        sf::Vector2f(mPlayerCfg.S * sin(vAngle), mPlayerCfg.S * cos(vAngle)), 0.0f);

    const sf::Color fillColor((*nextColorComp)(rng), (*nextColorComp)(rng), (*nextColorComp)(rng));
    const sf::Color outlineColor(mEnemyCfg.OR, mEnemyCfg.OG, mEnemyCfg.OB);
    enemy->cShape = make_shared<CShape>(mEnemyCfg.SR, vCount, fillColor, outlineColor, mEnemyCfg.OT);

    enemy->cCollision = make_shared<CCollision>(mEnemyCfg.CR);

    enemy->cScore = make_shared<CScore>(vCount * 100);

    enemy->cInput = make_shared<CInput>();
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> enemy)
{
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const sf::Vector2i& mousePos)
{
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
}
