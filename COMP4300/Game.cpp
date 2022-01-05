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
    if (mPlayer->getComponent<CInput>().left)
    {
        playerVelocity.x -= mPlayerCfg.S;
    }
    if (mPlayer->getComponent<CInput>().up)
    {
        playerVelocity.y -= mPlayerCfg.S;
    }
    if (mPlayer->getComponent<CInput>().right)
    {
        playerVelocity.x += mPlayerCfg.S;
    }
    if (mPlayer->getComponent<CInput>().down)
    {
        playerVelocity.y += mPlayerCfg.S;
    }
    mPlayer->getComponent<CTransform>().velocity = playerVelocity;

    for (auto& e : mEntityManager.getEntities())
    {
        e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;
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
                    mPlayer->getComponent<CInput>().up = true;
                    break;
                case sf::Keyboard::A:
                    mPlayer->getComponent<CInput>().left = true;
                    break;
                case sf::Keyboard::S:
                    mPlayer->getComponent<CInput>().down = true;
                    break;
                case sf::Keyboard::D:
                    mPlayer->getComponent<CInput>().right = true;
                    break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    mPlayer->getComponent<CInput>().up = false;
                    break;
                case sf::Keyboard::A:
                    mPlayer->getComponent<CInput>().left = false;
                    break;
                case sf::Keyboard::S:
                    mPlayer->getComponent<CInput>().down = false;
                    break;
                case sf::Keyboard::D:
                    mPlayer->getComponent<CInput>().right = false;
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
            if (event.mouseButton.button == sf::Mouse::Button::Left) mPlayer->getComponent<CInput>().shoot = true;
            mPlayer->getComponent<CInput>().mousePos = sf::Vector2i{ event.mouseButton.x, event.mouseButton.y };
        }
    }
}

void Game::sLifespan()
{
    for (auto& e : mEntityManager.getEntities())
    {
        if (e->getComponent<CLifeSpan>().has)
        {
            e->getComponent<CLifeSpan>().remaining--;
            if (e->getComponent<CLifeSpan>().remaining == 0) e->destroy();
        }
    }
}

void Game::sAttack()
{
    if (mPlayer->getComponent<CInput>().shoot)
    {
        spawnBullet(mPlayer, mPlayer->getComponent<CInput>().mousePos);
        mPlayer->getComponent<CInput>().shoot = false;
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
    auto& playerTransform = mPlayer->getComponent<CTransform>();
    playerTransform.pos.x = clamp(playerTransform.pos.x, (float)mPlayerCfg.SR, (float)(mWindowSize.x - mPlayerCfg.SR));
    playerTransform.pos.y = clamp(playerTransform.pos.y, (float)mPlayerCfg.SR, (float)(mWindowSize.y - mPlayerCfg.SR));

    for (auto& one : mEntityManager.getEntities())
    {
        if (one->tag() == EntityTag::Enemy)
        {
            auto& oneTransform = one->getComponent<CTransform>();
            if ((oneTransform.pos.x <= mEnemyCfg.SR) || (oneTransform.pos.x + mEnemyCfg.SR >= mWindowSize.x))
            {
                oneTransform.velocity.x *= -1;
            }
            if ((oneTransform.pos.y <= mEnemyCfg.SR) || (oneTransform.pos.y + mEnemyCfg.SR >= mWindowSize.y))
            {
                oneTransform.velocity.y *= -1;
            }
        }

        for (auto& other : mEntityManager.getEntities())
        {
            if (((one->tag() == EntityTag::Enemy) || (one->tag() == EntityTag::SmallEnemy)) && (other->tag() == EntityTag::Bullet)
                && collide(one, other))
            {
                mScore += one->getComponent<CScore>().score;
                one->destroy();
                other->destroy();
                if (one->tag() == EntityTag::Enemy) spawnSmallEnemies(one);
            }

            if ((one->tag() == EntityTag::Player) && (other->tag() == EntityTag::Enemy) && collide(one, other))
            {
                one->destroy();
                other->destroy();
                spawnPlayer();
            }
        }
    }
}

void Game::sRender()
{
    mRenderWindow.clear();
    for (auto& e : mEntityManager.getEntities())
    {
        e->getComponent<CTransform>().angle += 1.0f;
        e->getComponent<CShape>().circle.setRotation(e->getComponent<CTransform>().angle);
        e->getComponent<CShape>().circle.setPosition(e->getComponent<CTransform>().pos);

        if (e->getComponent<CLifeSpan>().has)
        {
            const auto fillColor = e->getComponent<CShape>().circle.getFillColor();
            const auto outlineColor = e->getComponent<CShape>().circle.getOutlineColor();
            const uint8_t alpha = ((float)e->getComponent<CLifeSpan>().remaining / e->getComponent<CLifeSpan>().total) * 255;
            e->getComponent<CShape>().circle.setFillColor({ fillColor.r, fillColor.g, fillColor.b, alpha });
            e->getComponent<CShape>().circle.setOutlineColor({ outlineColor.r, outlineColor.g, outlineColor.b, alpha });
        }

        mRenderWindow.draw(e->getComponent<CShape>().circle);
    }

    mText.setString(to_string(mScore));
    mRenderWindow.draw(mText);

    mRenderWindow.display();
}

bool Game::collide(const std::shared_ptr<Entity>& one, const std::shared_ptr<Entity>& other) const
{
    const auto distVec = one->getComponent<CTransform>().pos - other->getComponent<CTransform>().pos;
    const auto distSq = pow(distVec.x, 2) + pow(distVec.y, 2);
    return distSq <= pow(one->getComponent<CCollision>().radius + other->getComponent<CCollision>().radius, 2);
}

void Game::spawnPlayer()
{
    mPlayer = mEntityManager.addEntity(EntityTag::Player);
    
    mPlayer->addComponent<CTransform>(sf::Vector2f(mWindowSize.x / 2 - mPlayerCfg.SR, mWindowSize.y / 2 - mPlayerCfg.SR),
        sf::Vector2f{ 0, 0 }, 0.0f);

    const sf::Color fillColor(mPlayerCfg.FR, mPlayerCfg.FG, mPlayerCfg.FB);
    const sf::Color outlineColor(mPlayerCfg.OR, mPlayerCfg.OG, mPlayerCfg.OB);
    mPlayer->addComponent<CShape>(mPlayerCfg.SR, mPlayerCfg.V, fillColor, outlineColor, mPlayerCfg.OT);

    mPlayer->addComponent<CCollision>(mPlayerCfg.CR);

    mPlayer->addComponent<CInput>();
}

void Game::spawnEnemy()
{
    auto enemy = mEntityManager.addEntity(EntityTag::Enemy);

    const auto posX = (*nextEnemyPosX)(rng);
    const auto posY = (*nextEnemyPosY)(rng);
    const auto speed = (*nextEnemySpeed)(rng);
    const auto vAngle = (*nextAngle)(rng);
    const auto vCount = (*nextEnemyVCount)(rng);

    enemy->addComponent<CTransform>(sf::Vector2f(posX, posY),
        sf::Vector2f(mPlayerCfg.S * sin(vAngle), mPlayerCfg.S * cos(vAngle)), 0.0f);

    const sf::Color fillColor((*nextColorComp)(rng), (*nextColorComp)(rng), (*nextColorComp)(rng));
    const sf::Color outlineColor(mEnemyCfg.OR, mEnemyCfg.OG, mEnemyCfg.OB);
    enemy->addComponent<CShape>(mEnemyCfg.SR, vCount, fillColor, outlineColor, mEnemyCfg.OT);

    enemy->addComponent<CCollision>(mEnemyCfg.CR);

    enemy->addComponent<CScore>(vCount * 100);
}

void Game::spawnSmallEnemies(const std::shared_ptr<Entity>& enemy)
{
    const auto pointCount = enemy->getComponent<CShape>().circle.getPointCount();
    const float speed = sqrt(pow(enemy->getComponent<CTransform>().velocity.y, 2.0f) + pow(enemy->getComponent<CTransform>().velocity.y, 2.0f));

    for (size_t i = 0; i < pointCount; i++)
    {
        const auto smallEnemy = mEntityManager.addEntity(EntityTag::SmallEnemy);

        const float angle = i * (2 * numbers::pi) / pointCount;
        const auto velocity = sf::Vector2f{ sin(angle), cos(angle) } * speed;
        smallEnemy->addComponent<CTransform>(enemy->getComponent<CTransform>().pos, velocity, 0);
        
        smallEnemy->addComponent<CShape>(mEnemyCfg.SR / 2, pointCount, enemy->getComponent<CShape>().circle.getFillColor(),
            enemy->getComponent<CShape>().circle.getOutlineColor(), mEnemyCfg.OT);
    
        smallEnemy->addComponent<CCollision>(mEnemyCfg.CR / 2);

        smallEnemy->addComponent<CScore>(enemy->getComponent<CScore>().score * 2);

        smallEnemy->addComponent<CLifeSpan>(mEnemyCfg.L);
    }
}

void Game::spawnBullet(const std::shared_ptr<Entity>& entity, const sf::Vector2i& mousePos)
{
    const auto dirVec = sf::Vector2f(mousePos.x, mousePos.y) - entity->getComponent<CTransform>().pos;
    const auto bulletVel = dirVec * (mBulletCfg.S / sqrtf(pow(dirVec.x, 2) + pow(dirVec.y, 2)));

    auto bullet = mEntityManager.addEntity(EntityTag::Bullet);

    bullet->addComponent<CTransform>(entity->getComponent<CTransform>().pos, bulletVel, 0.0f);
    
    const sf::Color fillColor(mBulletCfg.FR, mBulletCfg.FG, mBulletCfg.FB);
    const sf::Color outlineColor(mBulletCfg.OR, mBulletCfg.OG, mBulletCfg.OB);
    bullet->addComponent<CShape>(mBulletCfg.SR, mBulletCfg.V, fillColor, outlineColor, mBulletCfg.OT);

    bullet->addComponent<CCollision>(mBulletCfg.CR);

    bullet->addComponent<CLifeSpan>(mBulletCfg.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
}
