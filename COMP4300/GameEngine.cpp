#include "GameEngine.h"
#include "SceneAnimation.h"
#include "SceneMenu.h"
#include "ScenePlay.h"
#include "SceneVision.h"

#include <SFML/Window/Event.hpp>

namespace goldenhand
{
    GameEngine::GameEngine() : mWindowSize{1280, 1024}, mCurrentScene{SceneId::Play}, mRunning{true}
    {
        mSceneMap[SceneId::Play] = std::make_unique<ScenePlay>(*this);
        mSceneMap[SceneId::Menu] = std::make_unique<SceneMenu>(*this);
        //mSceneMap[SceneId::Animation] = std::make_unique<SceneAnimation>(*this);
        //mSceneMap[SceneId::Vision] = std::make_unique<SceneVision>(*this);

        mRenderWindow.create(sf::VideoMode(mWindowSize.x, mWindowSize.y), "Shooter");
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

    void GameEngine::changeScene(const SceneId id)
    {
        mCurrentScene = id;
    }

    void GameEngine::createScene(const SceneId id)
    {
        switch (id)
        {
            case SceneId::Play:
                mSceneMap[id] = std::make_unique<ScenePlay>(*this);
            default:
                break;
        }

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
