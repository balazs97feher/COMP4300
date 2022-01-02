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
            sUserInput();
            sRender();
        }
        else
        {
            mEntityManager.update();

            sUserInput();
            sMovement();
            sCollision();
            sEnemySpawner();
            sAttack();
            sLifespan();

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
            mText.setPosition({ 10, 10 });
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
    sf::Event event;

    while (mRenderWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) mRunning = false;

        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    mPlayer->cInput->up = true;
                    break;
                case sf::Keyboard::A:
                    mPlayer->cInput->left = true;
                    break;
                case sf::Keyboard::S:
                    mPlayer->cInput->down = true;
                    break;
                case sf::Keyboard::D:
                    mPlayer->cInput->right = true;
                    break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    mPlayer->cInput->up = false;
                    break;
                case sf::Keyboard::A:
                    mPlayer->cInput->left = false;
                    break;
                case sf::Keyboard::S:
                    mPlayer->cInput->down = false;
                    break;
                case sf::Keyboard::D:
                    mPlayer->cInput->right = false;
                    break;
                case sf::Keyboard::P:
                    mPaused = !mPaused;
                    break;
                case sf::Keyboard::Escape:
                    mRunning = false;
                    break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Button::Left) mPlayer->cInput->shoot = true;
            mPlayer->cInput->mousePos = sf::Vector2i{ event.mouseButton.x, event.mouseButton.y };
        }
    }
}

void Game::sLifespan()
{
    for (auto& e : mEntityManager.getEntities())
    {
        if (e->cLifeSpan)
        {
            e->cLifeSpan->remaining--;
            if (e->cLifeSpan->remaining == 0) e->destroy();
        }
    }
}

void Game::sAttack()
{
    if (mPlayer->cInput->shoot)
    {
        spawnBullet(mPlayer, mPlayer->cInput->mousePos);
        mPlayer->cInput->shoot = false;
    }
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

    for (auto& one : mEntityManager.getEntities())
    {
        if (one->tag() == EntityTag::Enemy)
        {
            if ((one->cTransform->pos.x <= mEnemyCfg.SR) || (one->cTransform->pos.x + mEnemyCfg.SR >= mWindowSize.x))
            {
                one->cTransform->velocity.x *= -1;
            }
            if ((one->cTransform->pos.y <= mEnemyCfg.SR) || (one->cTransform->pos.y + mEnemyCfg.SR >= mWindowSize.y))
            {
                one->cTransform->velocity.y *= -1;
            }
        }

        for (auto& other : mEntityManager.getEntities())
        {
            if ((one->tag() == EntityTag::Enemy || one->tag() == EntityTag::SmallEnemy) && other->tag() == EntityTag::Bullet)
            {
                const auto distVec = one->cTransform->pos - other->cTransform->pos;
                const auto distSq = pow(distVec.x, 2) + pow(distVec.y, 2);
                if (distSq <= pow(one->cCollision->radius + other->cCollision->radius, 2))
                {
                    mScore += one->cScore->score;
                    one->destroy();
                    other->destroy();
                    if (one->tag() == EntityTag::Enemy) spawnSmallEnemies(one);
                }
            }
        }
    }
}

void Game::sRender()
{
    mRenderWindow.clear();
    for (auto& e : mEntityManager.getEntities())
    {
        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(e->cTransform->angle);
        e->cShape->circle.setPosition(e->cTransform->pos);

        if (e->cLifeSpan)
        {
            const auto fillColor = e->cShape->circle.getFillColor();
            const auto outlineColor = e->cShape->circle.getOutlineColor();
            const uint8_t alpha = ((float)e->cLifeSpan->remaining / e->cLifeSpan->total) * 255;
            e->cShape->circle.setFillColor({ fillColor.r, fillColor.g, fillColor.b, alpha });
            e->cShape->circle.setOutlineColor({ outlineColor.r, outlineColor.g, outlineColor.b, alpha });
        }

        mRenderWindow.draw(e->cShape->circle);
    }

    mText.setString(to_string(mScore));
    mRenderWindow.draw(mText);

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
}

void Game::spawnSmallEnemies(const std::shared_ptr<Entity>& enemy)
{
    const auto pointCount = enemy->cShape->circle.getPointCount();
    const float speed = sqrt(pow(enemy->cTransform->velocity.y, 2.0f) + pow(enemy->cTransform->velocity.y, 2.0f));

    for (size_t i = 0; i < pointCount; i++)
    {
        const auto smallEnemy = mEntityManager.addEntity(EntityTag::SmallEnemy);

        const float angle = i * (2 * numbers::pi) / pointCount;
        const auto velocity = sf::Vector2f{ sin(angle), cos(angle) } * speed;
        smallEnemy->cTransform = make_shared<CTransform>(enemy->cTransform->pos, velocity, 0);
        
        smallEnemy->cShape = make_shared<CShape>(mEnemyCfg.SR / 2, pointCount, enemy->cShape->circle.getFillColor(),
            enemy->cShape->circle.getOutlineColor(), mEnemyCfg.OT);
    
        smallEnemy->cCollision = make_shared<CCollision>(mEnemyCfg.CR / 2);

        smallEnemy->cScore = make_shared<CScore>(enemy->cScore->score * 2);

        smallEnemy->cLifeSpan = make_shared<CLifeSpan>(mEnemyCfg.L);
    }
}

void Game::spawnBullet(const std::shared_ptr<Entity>& entity, const sf::Vector2i& mousePos)
{
    const auto dirVec = sf::Vector2f(mousePos.x, mousePos.y) - entity->cTransform->pos;
    const auto bulletVel = dirVec * (mBulletCfg.S / sqrtf(pow(dirVec.x, 2) + pow(dirVec.y, 2)));

    auto bullet = mEntityManager.addEntity(EntityTag::Bullet);

    bullet->cTransform = make_shared<CTransform>(entity->cTransform->pos, bulletVel, 0.0f);
    
    const sf::Color fillColor(mBulletCfg.FR, mBulletCfg.FG, mBulletCfg.FB);
    const sf::Color outlineColor(mBulletCfg.OR, mBulletCfg.OG, mBulletCfg.OB);
    bullet->cShape = make_shared<CShape>(mBulletCfg.SR, mBulletCfg.V, fillColor, outlineColor, mBulletCfg.OT);

    bullet->cCollision = make_shared<CCollision>(mBulletCfg.CR);

    bullet->cLifeSpan = make_shared<CLifeSpan>(mBulletCfg.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
}
