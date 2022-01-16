#include "GameEngine.h"
#include "Physics.h"
#include "SceneVision.h"

#include <SFML/Window/Keyboard.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

SceneVision::SceneVision(GameEngine& engine) : Scene{ engine }, mBeam{sf::PrimitiveType::Lines, 2}
{
    registerAction(sf::Keyboard::Escape, ActionType::Quit);

    initialize();
}

void SceneVision::initialize()
{
    mLightSource.setFillColor(mBeamColor);
    mLightSource.setRadius(mRadius);
    mLightSource.setOrigin(mRadius / 2, mRadius / 2);

    mBeamOrigin = { mEngine.windowSize().x / 2.0f, mEngine.windowSize().y / 2.0f };
    mBeam[0] = sf::Vertex{ mBeamOrigin, sf::Color::Green };

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
    mBeam[1] = sf::Vertex{ { static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)}, mBeamColor };

    sPhysics();
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
    mEngine.drawToWindow(mBeam);
}

void SceneVision::sPhysics()
{
    vector<sf::Vector2f> intersections;

    for (const auto& shape : mShapes)
    {
        for (size_t i = 0; i < shape.getPointCount() - 1; i++)
        {
            if (const auto intersect = Physics::lineSegmentsIntersect(mBeam[0].position, mBeam[1].position,
                shape.getPoint(i), shape.getPoint(i + 1)))
            {
                intersections.push_back(intersect.value());
            }
        }
        if (const auto loopAroundIntersect = Physics::lineSegmentsIntersect(mBeam[0].position, mBeam[1].position,
            shape.getPoint(shape.getPointCount() - 1), shape.getPoint(0)))
        {
            intersections.push_back(loopAroundIntersect.value());
        }
    }
    if (intersections.empty()) return;

    sort(intersections.begin(), intersections.end(), [this](const sf::Vector2f& i1, const sf::Vector2f& i2) {
        return (pow(mBeamOrigin.x - i1.x, 2) + pow(mBeamOrigin.y - i1.y, 2)) < (pow(mBeamOrigin.x - i2.x, 2) + pow(mBeamOrigin.y - i2.y, 2));
    });

    mBeam[1] = sf::Vertex{ intersections[0], mBeamColor };
}
