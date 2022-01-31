#include "ConfigReader.h"
#include "Constants.h"
#include "GameEngine.h"
#include "SceneMenu.h"
#include "ScenePlatform.h"

#include <SFML/Window/Event.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

namespace goldenhand
{
    GameEngine::GameEngine() : mCurrentScene{ Constants::Scene::menu }, mRunning{true}
    {
    }

    void GameEngine::initialize(const std::string& settingsFile)
    {
        ConfigReader configuration{ settingsFile };

        string setting;
        while (!configuration.eof())
        {
            configuration >> setting;
            if (setting == "Window")
            {
                int wWidth, wHeight, frameLimit, fullScreenMode;
                string title;
                configuration >> wWidth >> wHeight >> frameLimit >> fullScreenMode >> title;

                const uint32_t wStyle = (fullScreenMode == 1) ? (sf::Style::Fullscreen | sf::Style::Default) : sf::Style::Default;
                mRenderWindow.create(sf::VideoMode(wWidth, wHeight), title, wStyle);
                mRenderWindow.setFramerateLimit(frameLimit);
                mWindowSize = mRenderWindow.getSize();
            }
        }

        mSceneMap[Constants::Scene::menu] = make_unique<SceneMenu>(*this);
        mSceneMap[Constants::Scene::platform] = make_unique<ScenePlatform>(*this);
    }

    void GameEngine::run()
    {
        while (mRunning)
        {
            sUserInput();
            update();
            sRender();
        }
    }

    void GameEngine::quit()
    {
        mRunning = false;
    }

    void GameEngine::changeScene(const std::string& id)
    {
        mCurrentScene = id;
    }

    void GameEngine::createScene(const std::string& id)
    {
        if (id == Constants::Scene::platform) mSceneMap[id] = make_unique<ScenePlatform>(*this);

        changeScene(id);
    }

    void GameEngine::setView(const sf::View& view)
    {
        mRenderWindow.setView(view);
    }

    sf::Vector2f GameEngine::mapPixelToCoords(const sf::Vector2i& point) const
    {
        return mRenderWindow.mapPixelToCoords(point);
    }

    sf::Vector2i GameEngine::mapCoordsToPixel(const sf::Vector2f& point) const
    {
        return mRenderWindow.mapCoordsToPixel(point);
    }

    sf::Vector2u GameEngine::windowSize() const
    {
        return mWindowSize;
    }

    void GameEngine::drawToWindow(const sf::Drawable& drawable)
    {
        mRenderWindow.draw(drawable);
    }

    sf::Vector2f GameEngine::mousePos() const
    {
        return mMousePos;
    }

    Scene* GameEngine::currentScene()
    {
        return mSceneMap[mCurrentScene].get();
    }

    void GameEngine::update()
    {
        currentScene()->update();
    }

    void GameEngine::sUserInput()
    {
        sf::Event event;

        while (mRenderWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) mRunning = false;

            else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
            {
                if (currentScene()->getKbdActionMap().contains(event.key.code))
                {
                    const auto actionType = currentScene()->getKbdActionMap().at(event.key.code);
                    currentScene()->sDoAction(Action(actionType, (event.type == sf::Event::KeyPressed) ? InputEventType::Pressed : InputEventType::Released));
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
            {
                if (currentScene()->getMouseActionMap().contains(event.mouseButton.button))
                {
                    const auto actionType = currentScene()->getMouseActionMap().at(event.mouseButton.button);
                    currentScene()->sDoAction(Action(actionType, (event.type == sf::Event::MouseButtonPressed) ? InputEventType::Pressed : InputEventType::Released));
                }
            }
            else  if (event.type == sf::Event::MouseMoved)
            {
                mMousePos = mRenderWindow.mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y });
            }
        }
    }

    void GameEngine::sRender()
    {
        mRenderWindow.clear();
        currentScene()->sRender();
        mRenderWindow.display();
    }
} // namespace goldenhand
