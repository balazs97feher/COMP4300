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
    mLightSource.setFillColor(sf::Color::Yellow);
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

    const sf::Vector2f windowSize{ static_cast<float>(mEngine.windowSize().x), static_cast<float>(mEngine.windowSize().y) };
    auto& windowFrame = mShapes.emplace_back(4);
    windowFrame.setPoint(0, { 0, 0 });
    windowFrame.setPoint(1, { 0, windowSize.y });
    windowFrame.setPoint(2, { windowSize.x, windowSize.y });
    windowFrame.setPoint(3, { windowSize.x, 0 });
    windowFrame.setFillColor(sf::Color::Transparent);
}

void SceneVision::update()
{
    const auto mousePos = mEngine.mousePos();
    mLightSource.setPosition(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    sBeamCast();
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

    sf::ConvexShape lightTriangle{ 3 };
    lightTriangle.setFillColor(mBeamColor);
    lightTriangle.setPoint(0, mLightSource.getPosition());
    for (size_t beamIdx = 0; beamIdx < mBeams.size() - 1; beamIdx++)
    {
        lightTriangle.setPoint(1, mBeams[beamIdx][1].position);
        lightTriangle.setPoint(2, mBeams[beamIdx + 1][1].position);
        mEngine.drawToWindow(lightTriangle);
    }

    lightTriangle.setPoint(1, mBeams[mBeams.size() - 1][1].position);
    lightTriangle.setPoint(2, mBeams[0][1].position);
    mEngine.drawToWindow(lightTriangle);

    mEngine.drawToWindow(mLightSource);
}

void SceneVision::sBeamCast()
{
    mBeams.clear();

    castBeamsToAllVertices();
    filterBlockedBeams();
    castBeamPairsAroundVertices();
    blockBeams();
    sortBeamsRadially();
}

void SceneVision::castBeamsToAllVertices()
{
    const auto sourcePos = mLightSource.getPosition();

    for (const auto& shape : mShapes)
    {
        for (size_t i = 0; i < shape.getPointCount(); i++) castBeam(sourcePos, shape.getPoint(i));

        const sf::Vector2f windowSize{ static_cast<float>(mEngine.windowSize().x), static_cast<float>(mEngine.windowSize().y) };
        castBeam(sourcePos, { 0, 0 });
        castBeam(sourcePos, { 0, windowSize.x });
        castBeam(sourcePos, { 0, windowSize.y });
        castBeam(sourcePos, { windowSize.x, windowSize.y });
    }
}

void SceneVision::filterBlockedBeams()
{
    vector<sf::VertexArray> unblockedBeams;

    for (size_t beamIdx = 0; beamIdx < mBeams.size(); beamIdx++)
    {
        auto intersections = beamIntersectsShapes(mBeams[beamIdx]);

        intersections.erase(unique(intersections.begin(), intersections.end()), intersections.end());

        if (intersections.size() == 1) unblockedBeams.push_back(move(mBeams[beamIdx]));
    }

    mBeams = unblockedBeams;
}

void SceneVision::castBeamPairsAroundVertices()
{
    const auto beamsToVisibleVertices{ move(mBeams) };
    mBeams.clear();

    constexpr float delta = numbers::pi / 10000;
    for (const auto& beam : beamsToVisibleVertices)
    {
        const auto beamVec = beam[1].position - beam[0].position;
        const auto angle = atan2(beamVec.y, beamVec.x);
        castBeam(angle - delta);
        castBeam(angle + delta);
    }
}

void SceneVision::blockBeams()
{
    for (size_t beamIdx = 0; beamIdx < mBeams.size(); beamIdx++)
    {
        auto intersections = beamIntersectsShapes(mBeams[beamIdx]);

        intersections.erase(unique(intersections.begin(), intersections.end()), intersections.end());

        const auto sourcePos = mLightSource.getPosition();
        sort(intersections.begin(), intersections.end(), [&sourcePos](const sf::Vector2f& i1, const sf::Vector2f& i2) {
            return (pow(sourcePos.x - i1.x, 2) + pow(sourcePos.y - i1.y, 2)) < (pow(sourcePos.x - i2.x, 2) + pow(sourcePos.y - i2.y, 2));
        });

        if (!intersections.empty()) mBeams[beamIdx][1] = sf::Vertex{ intersections[0], mBeamColor };
    }
}

void SceneVision::sortBeamsRadially()
{
    sort(mBeams.begin(), mBeams.end(), [](const sf::VertexArray& b1, const sf::VertexArray& b2) {
        const auto beamVec1 = b1[1].position - b1[0].position;
        const auto beamVec2 = b2[1].position - b2[0].position;
        return atan2(beamVec1.y, beamVec1.x) < atan2(beamVec2.y, beamVec2.x);
    });
}

std::vector<sf::Vector2f> SceneVision::beamIntersectsShapes(const sf::VertexArray& beam)
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

    return intersections;
}

void SceneVision::castBeam(const sf::Vector2f& start, const sf::Vector2f& end)
{
    auto& beam = mBeams.emplace_back(sf::PrimitiveType::Lines, 2);

    beam[0] = sf::Vertex{ start, mBeamColor };
    beam[1] = sf::Vertex{ end, mBeamColor };
}

void SceneVision::castBeam(const float angle)
{
    const auto sourcePos = mLightSource.getPosition();
    castBeam(sourcePos, { sourcePos.x + maxBeamSize * cos(angle), sourcePos.y + maxBeamSize * sin(angle) });
}
