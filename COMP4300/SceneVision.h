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
    
    void sPhysics();

private:
    const float mRadius = 5;
    sf::CircleShape mLightSource;
    std::vector<sf::ConvexShape> mShapes;

    const sf::Color mBeamColor = sf::Color::Green;
    sf::Vector2f mBeamOrigin;
    sf::VertexArray mBeam;
};

