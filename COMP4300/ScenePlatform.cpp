#include "Constants.h"
#include "GameEngine.h"
#include "ScenePlatform.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

ScenePlatform::ScenePlatform(goldenhand::GameEngine& engine) : Scene{ engine }, mAssetManager{ "./config/" }
{
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
        if (configType == "Background")
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
}

void ScenePlatform::update()
{
    mEntityManager.update();

    sAnimation();
}

void ScenePlatform::sDoAction(const goldenhand::Action action)
{
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

void ScenePlatform::sAnimation()
{
    for (const auto& entity : mEntityManager.getEntities())
    {
        auto& animation = entity->getComponent<CAnimation>();
        if (animation.has) mAssetManager.getAnimation(animation.animation).update();
    }
}
