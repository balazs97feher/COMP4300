#include "Constants.h"
#include "GameEngine.h"
#include "SceneAnimation.h"
#include "SceneMenu.h"

#include <SFML/Window/Event.hpp>

namespace goldenhand
{
    GameEngine::GameEngine() : mWindowSize{1280, 1024}, mCurrentScene{ Constants::Scene::menu }, mRunning{true}
    {
        mSceneMap[Constants::Scene::menu] = std::make_unique<SceneMenu>(*this);
        mSceneMap[Constants::Scene::level1] = std::make_unique<SceneAnimation>(*this);

        mRenderWindow.create(sf::VideoMode(mWindowSize.x, mWindowSize.y), "GoldenHand");
        mRenderWindow.setFramerateLimit(60);
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

    void GameEngine::changeScene(const std::string id)
    {
        mCurrentScene = id;
    }

    void GameEngine::createScene(const std::string id)
    {
        if (id == Constants::Scene::level1) mSceneMap[id] = std::make_unique<SceneAnimation>(*this);

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
