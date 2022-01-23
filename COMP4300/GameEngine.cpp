#include "Constants.h"
#include "GameEngine.h"
#include "SceneAnimation.h"
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
        const fs::path configFile{ settingsFile };
        if (!fs::exists(configFile))
        {
            cerr << "Settings file " << configFile.string() << " does not exit." << endl;
            exit(-1);
        }

        ifstream configuration{ configFile };
        if (!configuration.is_open())
        {
            cerr << "Failed to open settings file." << endl;
            exit(-1);
        }

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
        mSceneMap[Constants::Scene::level1] = make_unique<SceneAnimation>(*this);
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
        if (id == Constants::Scene::level1) mSceneMap[id] = make_unique<SceneAnimation>(*this);

        changeScene(id);
    }

    sf::Vector2u GameEngine::windowSize() const
    {
        return mWindowSize;
    }

    void GameEngine::drawToWindow(const sf::Drawable& drawable)
    {
        mRenderWindow.draw(drawable);
    }

    sf::Vector2i GameEngine::mousePos() const
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

            else  if (event.type == sf::Event::MouseMoved)
            {
                mMousePos = { event.mouseMove.x, event.mouseMove.y };
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
