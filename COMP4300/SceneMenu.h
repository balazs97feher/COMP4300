#pragma once

#include "Scene.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include <functional>

class SceneMenu : public Scene
{
public:
    SceneMenu(GameEngine& engine);

    virtual void initialize() override;

    virtual void update() override;
    virtual void sDoAction(const Action action) override;
    virtual void sRender() override;

private:
    const int mCharSize = 48;
    const sf::Color mInactiveColor;
    const sf::Color mSelectedColor;
    sf::Font mFont;
    sf::Text mContinueText;
    sf::Text mRestartText;
    int mSelectedIdx;
};

