#pragma once

#include "Scene.h"
#include "ScenePlay.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>

class GameEngine
{
public:
    GameEngine();

    void run();
    void quit();

    const sf::Vector2u& windowSize() const;
    void drawToWindow(const sf::Drawable& drawable);

private:
    sf::Vector2u mWindowSize;
    sf::RenderWindow mRenderWindow;

    SceneId mCurrentScene;
    std::unordered_map<SceneId, std::unique_ptr<Scene>> mSceneMap;
    Scene* currentScene();
    bool mRunning;

    void update();
    void sUserInput();
    void sRender();
};

