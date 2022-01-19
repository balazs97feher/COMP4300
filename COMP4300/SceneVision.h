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

    float maxRaySize;
    std::vector<sf::VertexArray> mRays;
    const sf::Color mRayColor = sf::Color::White;
    std::vector<sf::Vector2f> mIntersections;
    
    void sRayCast();

    void castRaysToAllVertices();
    void filterBlockedRays();
    void castRayPairsAroundVertices();
    void blockRays();
    void sortRaysRadially();
    std::vector<sf::Vector2f> rayIntersectsShapes(const sf::VertexArray& ray);

    void castRay(const sf::Vector2f& start, const sf::Vector2f& end);
    void castRay(const float angle);
};

