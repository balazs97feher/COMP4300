#pragma once

#include "Scene.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>

namespace goldenhand
{
    class GameEngine
    {
    public:
        GameEngine();

        void initialize(const std::string& settingsFile);

        void run();
        void quit();

        void changeScene(const std::string& id);
        void createScene(const std::string& id);

        sf::Vector2u windowSize() const;
        void drawToWindow(const sf::Drawable& drawable);
        sf::Vector2i mousePos() const;

    private:
        sf::Vector2u mWindowSize;
        sf::RenderWindow mRenderWindow;
        sf::Vector2i mMousePos;

        std::string mCurrentScene;
        std::unordered_map<std::string, std::unique_ptr<Scene>> mSceneMap;
        Scene* currentScene();
        bool mRunning;

        void update();
        void sUserInput();
        void sRender();
    };
} // namespace goldenhand
