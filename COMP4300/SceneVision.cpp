#include "GameEngine.h"
#include "Physics.h"
#include "SceneVision.h"

#include <SFML/Window/Keyboard.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numbers>

namespace fs = std::filesystem;
using namespace std;

SceneVision::SceneVision(GameEngine& engine) : Scene{ engine }
{
    registerAction(sf::Keyboard::Escape, ActionType::Quit);

    initialize();
}

void SceneVision::initialize()
{
    mLightSource.setFillColor(mBeamColor);
    mLightSource.setRadius(mRadius);
    mLightSource.setOrigin(mRadius / 2, mRadius / 2);

    maxBeamSize = sqrt(pow(mEngine.windowSize().x, 2) + pow(mEngine.windowSize().y, 2));

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

    sBeamCast();
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

    for (const auto& beam : mBeams) mEngine.drawToWindow(beam);

    sf::CircleShape dot;
    dot.setRadius(2);
    dot.setFillColor(sf::Color::Green);
    for (const auto& intersection : mIntersections)
    {
        dot.setPosition(intersection);
        mEngine.drawToWindow(dot);
    }
}

void SceneVision::sBeamCast()
{
    constexpr int beamCount = 16;
    mBeams.clear();

    for (size_t i = 0; i < beamCount; i++) castBeam(i * 2 * numbers::pi / beamCount);
}

void SceneVision::sPhysics()
{
    mIntersections.clear();

    for (auto& beam : mBeams)
    {
        vector<sf::Vector2f> intersections;

        for (const auto& shape : mShapes)
        {
            for (size_t i = 0; i < shape.getPointCount() - 1; i++)
            {
                if (const auto intersect = Physics::lineSegmentsIntersect(beam[0].position, beam[1].position,
                    shape.getPoint(i), shape.getPoint(i + 1)))
                {
                    intersections.push_back(intersect.value());
                }
            }
            if (const auto loopAroundIntersect = Physics::lineSegmentsIntersect(beam[0].position, beam[1].position,
                shape.getPoint(shape.getPointCount() - 1), shape.getPoint(0)))
            {
                intersections.push_back(loopAroundIntersect.value());
            }
        }

        const auto sourcePos = mLightSource.getPosition();
        sort(intersections.begin(), intersections.end(), [&sourcePos](const sf::Vector2f& i1, const sf::Vector2f& i2) {
            return (pow(sourcePos.x - i1.x, 2) + pow(sourcePos.y - i1.y, 2)) < (pow(sourcePos.x - i2.x, 2) + pow(sourcePos.y - i2.y, 2));
        });

        if (!intersections.empty()) beam[1] = sf::Vertex{ intersections[0], mBeamColor };

        mIntersections.insert(mIntersections.end(), intersections.begin(), intersections.end());
    }
}

void SceneVision::castBeam(const float angle)
{
    auto& beam = mBeams.emplace_back(sf::PrimitiveType::Lines, 2);

    const auto sourcePos = mLightSource.getPosition();
    beam[0] = sf::Vertex{ sourcePos, mBeamColor };
    beam[1] = sf::Vertex{ {sourcePos.x + maxBeamSize * cos(angle), sourcePos.y + maxBeamSize * sin(angle)}, mBeamColor };
}
