#pragma once

#include "Scene.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <vector>

class SceneVision : public Scene
{
public:
    SceneVision(GameEngine& engine);

    virtual void initialize() override;

    virtual void update() override;
    virtual void sDoAction(const Action action) override;
    virtual void sRender() override;

private:
    const float mRadius = 5;
    sf::CircleShape mLightSource;
    std::vector<sf::ConvexShape> mShapes;

    float maxBeamSize;
    std::vector<sf::VertexArray> mBeams;
    const sf::Color mBeamColor = sf::Color::White;
    std::vector<sf::Vector2f> mIntersections;
    
    void sBeamCast();

    void castBeamsToAllVertices();
    void filterBlockedBeams();
    void castBeamPairsAroundVertices();
    void blockBeams();
    void sortBeamsRadially();
    std::vector<sf::Vector2f> beamIntersectsShapes(const sf::VertexArray& beam);

    void castBeam(const sf::Vector2f& start, const sf::Vector2f& end);
    void castBeam(const float angle);
};

