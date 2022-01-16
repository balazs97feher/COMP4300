#pragma once

#include "Scene.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

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
};
