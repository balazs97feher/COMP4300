#pragma once

#include "Scene.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>

class GameEngine
{
public:
    GameEngine();

    void run();
    void quit();

    void changeScene(const SceneId id);
    void createScene(const SceneId id);

    sf::Vector2u windowSize() const;
    void drawToWindow(const sf::Drawable& drawable);
    sf::Vector2i mousePos() const;

private:
    sf::Vector2u mWindowSize;
    sf::RenderWindow mRenderWindow;
    sf::Vector2i mMousePos;

    SceneId mCurrentScene;
    std::unordered_map<SceneId, std::unique_ptr<Scene>> mSceneMap;
    Scene* currentScene();
    bool mRunning;

    void update();
    void sUserInput();
    void sRender();
};

