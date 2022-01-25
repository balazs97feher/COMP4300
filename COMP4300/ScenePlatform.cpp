#include "ConfigReader.h"
#include "Constants.h"
#include "GameEngine.h"
#include "ScenePlatform.h"

#include <SFML/Window/Keyboard.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

ScenePlatform::ScenePlatform(goldenhand::GameEngine& engine) : Scene{ engine }, mAssetManager{ "./config/" }, mPhysics({ 0.f, .2f })
{
    using namespace goldenhand;

    registerAction(sf::Keyboard::A, ActionType::MoveLeft);
    registerAction(sf::Keyboard::D, ActionType::MoveRight);
    registerAction(sf::Keyboard::W, ActionType::MoveUp);
    registerAction(sf::Keyboard::Escape, ActionType::Quit);

    initialize();
}

void ScenePlatform::initialize()
{
    mAssetManager.loadAssets();

    goldenhand::ConfigReader configuration("./config/level1.txt");

    string configType;
    configuration >> configType;
    while (!configuration.eof())
    {
        string name;
        float x, y;
        if (configType == "Player")
        {
            configuration >> mPlayerConfig.speed;
        }
        else if (configType == "Background")
        {
            configuration >> name;
            auto background = mEntityManager.addEntity(EntityTag::Decoration);
            background->addComponent<CTransform>(sf::Vector2f{ mEngine.windowSize().x / 2.f, mEngine.windowSize().y / 2.f }, sf::Vector2f{ 0, 0 }, 0.f);
            background->addComponent<CAnimation>(name);
            auto& animation = mAssetManager.getAnimation(name);
            animation.getSprite().setScale(static_cast<float>(mEngine.windowSize().x) / animation.getSize().x,
                static_cast<float>(mEngine.windowSize().y) / animation.getSize().y);
        }
        else if (configType == "Tile")
        {
            configuration >> name >> x >> y;
            auto tile = mEntityManager.addEntity(EntityTag::Tile);
            tile->addComponent<CTransform>(sf::Vector2f{ x, y }, sf::Vector2f{ 0, 0 }, 0.f);
            tile->addComponent<CAnimation>(name);
            tile->addComponent<CBoundingBox>(mAssetManager.getAnimation(name).getSize());
        }

        configuration >> configType;
    }

    spawnPlayer();
}

void ScenePlatform::update()
{
    if (!mHasEnded)
    {
        mEntityManager.update();

        sMovement();
        sPhysics();
        sAnimation();
        sRender();

        mCurrentFrame++;
    }
}

void ScenePlatform::sDoAction(const goldenhand::Action action)
{
    using namespace goldenhand;

    auto& velocity = mPlayer->getComponent<CTransform>().velocity;

    switch (action.getType())
    {
    case ActionType::Quit:
        if (action.getEventType() == InputEventType::Released) mEngine.changeScene(Constants::Scene::menu);
        return;
    case ActionType::MoveLeft:
        if (action.getEventType() == InputEventType::Pressed) velocity = { -mPlayerConfig.speed, velocity.y };
        else velocity = sf::Vector2f{ 0.f, velocity.y };
        break;
    case ActionType::MoveRight:
        if (action.getEventType() == InputEventType::Pressed) velocity = { mPlayerConfig.speed, velocity.y };
        else velocity = sf::Vector2f{ 0.f, velocity.y };
        break;
    case ActionType::MoveUp:
        if (action.getEventType() == InputEventType::Released && velocity.y == 0) velocity.y = - 10;
        break;
    default:
        break;
    }
}

void ScenePlatform::sRender()
{
    for (const auto& entity : mEntityManager.getEntities())
    {
        auto& animation = entity->getComponent<CAnimation>();
        if (animation.has)
        {
            auto& sprite = mAssetManager.getAnimation(entity->getComponent<CAnimation>().animation).getSprite();
            sprite.setPosition(entity->getComponent<CTransform>().pos);
            mEngine.drawToWindow(sprite);
        }
    }
}

void ScenePlatform::sMovement()
{
    mPlayer->getComponent<CTransform>().velocity += mPhysics.gravity();

    mPlayer->getComponent<CTransform>().prevPos = mPlayer->getComponent<CTransform>().pos;
    mPlayer->getComponent<CTransform>().pos += mPlayer->getComponent<CTransform>().velocity;

    if (mPlayer->getComponent<CTransform>().pos.x > mPlayer->getComponent<CTransform>().prevPos.x)
    {
        mPlayer->getComponent<CTransform>().angle = 1;
    }
    else if (mPlayer->getComponent<CTransform>().pos.x < mPlayer->getComponent<CTransform>().prevPos.x)
    {
        mPlayer->getComponent<CTransform>().angle = -1;
    }
}

void ScenePlatform::sPhysics()
{
    for (auto& one : mEntityManager.getEntities())
    {
        if (one->tag() == EntityTag::Tile)
        {
            if (const auto overlap = goldenhand::Physics::boxesOverlap(mPlayer->getComponent<CTransform>().pos, mPlayer->getComponent<CBoundingBox>().halfSize,
                one->getComponent<CTransform>().pos, one->getComponent<CBoundingBox>().halfSize))
            {
                auto& transform = mPlayer->getComponent<CTransform>();
                transform.pos.y -= overlap->height;
                transform.velocity = { transform.velocity.x, 0.f};
                break;
            }
        }
    }
}

void ScenePlatform::sAnimation()
{
    for (const auto& entity : mEntityManager.getEntities())
    {
        if (entity->tag() == EntityTag::Player) continue;

        auto& animation = entity->getComponent<CAnimation>();
        if (animation.has) mAssetManager.getAnimation(animation.animation).update();
    }

    if (mPlayer->getComponent<CTransform>().velocity.y != 0) mPlayer->getComponent<CAnimation>().animation = Constants::Animation::megaman_jumping;
    else if (mPlayer->getComponent<CTransform>().velocity.x == 0) mPlayer->getComponent<CAnimation>().animation = Constants::Animation::megaman_standing;
    else mPlayer->getComponent<CAnimation>().animation = Constants::Animation::megaman_running;

    auto& animation = mAssetManager.getAnimation(mPlayer->getComponent<CAnimation>().animation);

    if (mPlayer->getComponent<CTransform>().angle == 1) animation.getSprite().setScale(1.f, 1.f);
    else animation.getSprite().setScale(-1.f, 1.f);

    animation.update();
}

void ScenePlatform::spawnPlayer()
{
    mPlayer = mEntityManager.addEntity(EntityTag::Player);

    mPlayer->addComponent<CTransform>(sf::Vector2f{ 200, 400 }, sf::Vector2f{ 0, 0 }, 1.0f);
    mPlayer->addComponent<CAnimation>(Constants::Animation::megaman_standing);
    mPlayer->addComponent<CBoundingBox>(mAssetManager.getAnimation(Constants::Animation::megaman_standing).getSize());
}
