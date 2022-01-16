#include "GameEngine.h"
#include "SceneVision.h"

#include <SFML/Window/Keyboard.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

SceneVision::SceneVision(GameEngine& engine) : Scene{ engine }
{
    registerAction(sf::Keyboard::Escape, ActionType::Quit);

    initialize();
}

void SceneVision::initialize()
{
    mLightSource.setFillColor({ 255, 255, 255 });
    mLightSource.setRadius(mRadius);
    mLightSource.setOrigin(mRadius / 2, mRadius / 2);

    const fs::path configFile{ "./config/config_vision.txt" };
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

    int pointCount;
    while (!configuration.eof())
    {
        configuration >> pointCount;
        vector<float> pointCoords(pointCount * 2);
        for (auto& coord : pointCoords) configuration >> coord;

        auto& newShape = mShapes.emplace_back(pointCount);
        for (size_t i = 0; i < pointCount; i++)
        {
            newShape.setPoint(i, { pointCoords[i * 2], pointCoords[i * 2 + 1] });
        }
        newShape.setFillColor(sf::Color::Red);
    }
}

void SceneVision::update()
{
    const auto mousePos = mEngine.mousePos();
    mLightSource.setPosition(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

void SceneVision::sDoAction(const Action action)
{
    switch (action.getType())
    {
        case ActionType::Quit:
            if (action.getEventType() == InputEventType::Released) mEngine.changeScene(SceneId::Menu);
            return;
        default:
            break;
    }
}

void SceneVision::sRender()
{
    for (const auto& shape : mShapes) mEngine.drawToWindow(shape);
    mEngine.drawToWindow(mLightSource);
}
