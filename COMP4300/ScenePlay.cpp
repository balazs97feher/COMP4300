#include "ScenePlay.h"
#include "GameEngine.h"

#include <SFML/Window.hpp>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numbers>

namespace fs = std::filesystem;
using namespace std;

ScenePlay::ScenePlay(GameEngine& engine) : Scene{engine}, mDrawBB{false}
{
    registerAction(sf::Keyboard::W, ActionType::MoveUp);
    registerAction(sf::Keyboard::A, ActionType::MoveLeft);
    registerAction(sf::Keyboard::S, ActionType::MoveDown);
    registerAction(sf::Keyboard::D, ActionType::MoveRight);
    registerAction(sf::Keyboard::Space, ActionType::Shoot);
    registerAction(sf::Keyboard::Escape, ActionType::Quit);
    registerAction(sf::Keyboard::B, ActionType::ToggleBBDraw);

    initialize();
}

void ScenePlay::initialize()
{
    const fs::path configFile{ "./config/config_assignment02.txt" };
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
        if (configType == "Font")
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

            nextEnemyPosX = make_unique<std::uniform_int_distribution<>>(0, mEngine.windowSize().x - mEnemyCfg.SR);
            nextEnemyPosY = make_unique<std::uniform_int_distribution<>>(0, mEngine.windowSize().y - mEnemyCfg.SR);
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

void ScenePlay::update()
{
    if (!mHasEnded)
    {
        mEntityManager.update();

        sMovement();
        sCollision();
        sEnemySpawner();
        sLifespan();

        mCurrentFrame++;
    }
}

void ScenePlay::sDoAction(const Action action)
{
    auto newVelocity = mPlayer->getComponent<CTransform>().velocity;
    switch (action.getType())
    {
        case ActionType::Quit:
            if (action.getEventType() == InputEventType::Released) mEngine.changeScene(SceneId::Menu);
            return;
        case ActionType::ToggleBBDraw:
            if (action.getEventType() == InputEventType::Released) mDrawBB = !mDrawBB;
            return;
        case ActionType::MoveUp:
            if (action.getEventType() == InputEventType::Pressed) newVelocity.y = max(-mPlayerCfg.S, newVelocity.y - mPlayerCfg.S);
            else newVelocity.y =  (newVelocity.y == 0) ? 0 : min(mPlayerCfg.S, newVelocity.y + mPlayerCfg.S);
            break;
        case ActionType::MoveLeft:
            if (action.getEventType() == InputEventType::Pressed) newVelocity.x = max(-mPlayerCfg.S, newVelocity.x - mPlayerCfg.S);
            else newVelocity.x = (newVelocity.x == 0) ? 0 : min(mPlayerCfg.S, newVelocity.x + mPlayerCfg.S);
            break;
        case ActionType::MoveDown:
            if (action.getEventType() == InputEventType::Pressed) newVelocity.y = min(mPlayerCfg.S, newVelocity.y + mPlayerCfg.S);
            else newVelocity.y = (newVelocity.y == 0) ? 0 : max(-mPlayerCfg.S, newVelocity.y - mPlayerCfg.S);
            break;
        case ActionType::MoveRight:
            if (action.getEventType() == InputEventType::Pressed) newVelocity.x = min(mPlayerCfg.S, newVelocity.x + mPlayerCfg.S);
            else newVelocity.x = (newVelocity.x == 0) ? 0 : max(-mPlayerCfg.S, newVelocity.x - mPlayerCfg.S);
            break;
        case ActionType::Shoot:
            spawnBullet(mPlayer, mEngine.mousePos());
        default:
            break;
    }

    mPlayer->getComponent<CTransform>().velocity = newVelocity;
}

void ScenePlay::sRender()
{
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

        mEngine.drawToWindow(e->getComponent<CShape>().circle);

        if (mDrawBB && e->getComponent<CBoundingBox>().has)
        {
            e->getComponent<CBoundingBox>().rect.setPosition(e->getComponent<CTransform>().pos);
            mEngine.drawToWindow(e->getComponent<CBoundingBox>().rect);
        }
    }

    mText.setString(to_string(mScore));
    mEngine.drawToWindow(mText);
}

void ScenePlay::sMovement()
{
    for (auto& e : mEntityManager.getEntities())
    {
        e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;
    }
}

void ScenePlay::sLifespan()
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

void ScenePlay::sEnemySpawner()
{
    if ((mCurrentFrame % mEnemyCfg.SI) == 0)
    {
        spawnEnemy();
    }
}

void ScenePlay::sCollision()
{
    auto& playerTransform = mPlayer->getComponent<CTransform>();
    playerTransform.pos.x = clamp(playerTransform.pos.x, (float)mPlayerCfg.SR, (float)(mEngine.windowSize().x - mPlayerCfg.SR));
    playerTransform.pos.y = clamp(playerTransform.pos.y, (float)mPlayerCfg.SR, (float)(mEngine.windowSize().y - mPlayerCfg.SR));

    for (auto& one : mEntityManager.getEntities())
    {
        if (one->tag() == EntityTag::Enemy)
        {
            auto& oneTransform = one->getComponent<CTransform>();
            if ((oneTransform.pos.x <= mEnemyCfg.SR) || (oneTransform.pos.x + mEnemyCfg.SR >= mEngine.windowSize().x))
            {
                oneTransform.velocity.x *= -1;
            }
            if ((oneTransform.pos.y <= mEnemyCfg.SR) || (oneTransform.pos.y + mEnemyCfg.SR >= mEngine.windowSize().y))
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

bool ScenePlay::collide(const std::shared_ptr<Entity>& one, const std::shared_ptr<Entity>& other) const
{
    const auto distVec = one->getComponent<CTransform>().pos - other->getComponent<CTransform>().pos;
    const auto dx = abs(distVec.x);
    const auto dy = abs(distVec.y);

    const auto overlap = sf::Vector2f{ (one->getComponent<CBoundingBox>().halfSize.x + other->getComponent<CBoundingBox>().halfSize.x - dx),
        (one->getComponent<CBoundingBox>().halfSize.y + other->getComponent<CBoundingBox>().halfSize.y - dy) };

    return overlap.x > 0 && overlap.y > 0;
}

void ScenePlay::spawnPlayer()
{
    mPlayer = mEntityManager.addEntity(EntityTag::Player);

    mPlayer->addComponent<CTransform>(sf::Vector2f(mEngine.windowSize().x / 2 - mPlayerCfg.SR, mEngine.windowSize().y / 2 - mPlayerCfg.SR),
        sf::Vector2f{ 0, 0 }, 0.0f);

    const sf::Color fillColor(mPlayerCfg.FR, mPlayerCfg.FG, mPlayerCfg.FB);
    const sf::Color outlineColor(mPlayerCfg.OR, mPlayerCfg.OG, mPlayerCfg.OB);
    mPlayer->addComponent<CShape>(mPlayerCfg.SR, mPlayerCfg.V, fillColor, outlineColor, mPlayerCfg.OT);

    mPlayer->addComponent<CBoundingBox>(sf::Vector2f{ mPlayerCfg.CR * 2.0f, mPlayerCfg.CR * 2.0f });
}

void ScenePlay::spawnEnemy()
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

    enemy->addComponent<CBoundingBox>(sf::Vector2f{ mEnemyCfg.CR * 2.0f, mEnemyCfg.CR * 2.0f });

    enemy->addComponent<CScore>(vCount * 100);
}

void ScenePlay::spawnSmallEnemies(const std::shared_ptr<Entity>& enemy)
{
    const auto pointCount = enemy->getComponent<CShape>().circle.getPointCount();
    const float speed = sqrt(pow(enemy->getComponent<CTransform>().velocity.y, 2.0f) + pow(enemy->getComponent<CTransform>().velocity.y, 2.0f));

    for (size_t i = 0; i < pointCount; i++)
    {
        const auto smallEnemy = mEntityManager.addEntity(EntityTag::SmallEnemy);

        const float angle = i * (2 * numbers::pi) / pointCount;
        const auto velocity = sf::Vector2f{ sin(angle), cos(angle) } *speed;
        smallEnemy->addComponent<CTransform>(enemy->getComponent<CTransform>().pos, velocity, 0);

        smallEnemy->addComponent<CShape>(mEnemyCfg.SR / 2, pointCount, enemy->getComponent<CShape>().circle.getFillColor(),
            enemy->getComponent<CShape>().circle.getOutlineColor(), mEnemyCfg.OT);

        smallEnemy->addComponent<CBoundingBox>(sf::Vector2f{ (float)mEnemyCfg.CR, (float)mEnemyCfg.CR });

        smallEnemy->addComponent<CScore>(enemy->getComponent<CScore>().score * 2);

        smallEnemy->addComponent<CLifeSpan>(mEnemyCfg.L);
    }
}

void ScenePlay::spawnBullet(const std::shared_ptr<Entity>& entity, const sf::Vector2i& mousePos)
{
    const auto dirVec = sf::Vector2f(mousePos.x, mousePos.y) - entity->getComponent<CTransform>().pos;
    const auto bulletVel = dirVec * (mBulletCfg.S / sqrtf(pow(dirVec.x, 2) + pow(dirVec.y, 2)));

    auto bullet = mEntityManager.addEntity(EntityTag::Bullet);

    bullet->addComponent<CTransform>(entity->getComponent<CTransform>().pos, bulletVel, 0.0f);

    const sf::Color fillColor(mBulletCfg.FR, mBulletCfg.FG, mBulletCfg.FB);
    const sf::Color outlineColor(mBulletCfg.OR, mBulletCfg.OG, mBulletCfg.OB);
    bullet->addComponent<CShape>(mBulletCfg.SR, mBulletCfg.V, fillColor, outlineColor, mBulletCfg.OT);

    bullet->addComponent<CBoundingBox>(sf::Vector2f{ mBulletCfg.CR * 2.0f, mBulletCfg.CR * 2.0f });

    bullet->addComponent<CLifeSpan>(mBulletCfg.L);
}