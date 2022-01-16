#include "GameEngine.h"
#include "SceneVision.h"

#include <SFML/Window/Keyboard.hpp>

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
    mEngine.drawToWindow(mLightSource);
}
