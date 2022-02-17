#pragma once

#include "Scene.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

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

        void changeScene(const std::string_view id);
        void createScene(const std::string_view id);

        const sf::View& getView() const;
        void setView(const sf::View& view);
        sf::Vector2f mapPixelToCoords(const sf::Vector2i& point) const;
        sf::Vector2i mapCoordsToPixel(const sf::Vector2f& point) const;

        sf::Vector2u windowSize() const;
        void drawToWindow(const sf::Drawable& drawable);

        /*
        * Position of the cursor in the world's coordinate system, use mapCoordsToPixel if neccessary
        */
        sf::Vector2f mousePos() const;

    private:
        sf::Vector2u mWindowSize;
        sf::RenderWindow mRenderWindow;
        sf::Vector2f mMousePos;

        std::string mCurrentScene;
        std::unordered_map<std::string_view, std::unique_ptr<Scene>> mSceneMap;
        Scene* currentScene();
        bool mRunning;

        void update();
        void sUserInput();
        void sRender();
    };
} // namespace goldenhand
