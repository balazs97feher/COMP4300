#include "Constants.h"
#include "GameEngine.h"
#include "ScenePlatform.h"

#include <SFML/Window/Keyboard.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

ScenePlatform::ScenePlatform(goldenhand::GameEngine& engine) : Scene{ engine }, mAssetManager{ "./config/" }
{
    using namespace goldenhand;

    registerAction(sf::Keyboard::A, ActionType::MoveLeft);
    registerAction(sf::Keyboard::D, ActionType::MoveRight);
    registerAction(sf::Keyboard::Escape, ActionType::Quit);

    initialize();
}

void ScenePlatform::initialize()
{
    mAssetManager.loadAssets();

    const fs::path configFile{ "./config/level1.txt"};
    if (!fs::exists(configFile))
    {
        cerr << "Config file " << configFile.string() << " does not exit." << endl;
        exit(-1);
    }

    ifstream configuration{ configFile };
    if (!configuration.is_open())
    {
        cerr << "Failed to open config file." << endl;
        exit(-1);
    }

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
        }
        else if (configType == "Tile")
        {
            configuration >> name >> x >> y;
            auto tile = mEntityManager.addEntity(EntityTag::Tile);
            tile->addComponent<CTransform>(sf::Vector2f{ x, y }, sf::Vector2f{ 0, 0 }, 0.f);
            tile->addComponent<CAnimation>(name);
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
        sAnimation();
        sRender();

        mCurrentFrame++;
    }
}

void ScenePlatform::sDoAction(const goldenhand::Action action)
{
    using namespace goldenhand;

    switch (action.getType())
    {
    case ActionType::Quit:
        if (action.getEventType() == InputEventType::Released) mEngine.changeScene(Constants::Scene::menu);
        return;
    case ActionType::MoveLeft:
        if (action.getEventType() == InputEventType::Pressed) mPlayer->getComponent<CTransform>().velocity = sf::Vector2f{ -mPlayerConfig.speed, 0 };
        else mPlayer->getComponent<CTransform>().velocity = sf::Vector2f{ 0, 0 };
        break;
    case ActionType::MoveRight:
        if (action.getEventType() == InputEventType::Pressed) mPlayer->getComponent<CTransform>().velocity = sf::Vector2f{ mPlayerConfig.speed, 0 };
        else mPlayer->getComponent<CTransform>().velocity = sf::Vector2f{ 0, 0 };
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

void ScenePlatform::sAnimation()
{
    for (const auto& entity : mEntityManager.getEntities())
    {
        if (entity->tag() == EntityTag::Player) continue;

        auto& animation = entity->getComponent<CAnimation>();
        if (animation.has) mAssetManager.getAnimation(animation.animation).update();
    }

    if (mPlayer->getComponent<CTransform>().velocity.x == 0) mPlayer->getComponent<CAnimation>().animation = Constants::Animation::megaman_standing;
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
}
