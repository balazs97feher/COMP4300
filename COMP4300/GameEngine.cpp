#include "GameEngine.h"

#include <SFML/Window/Event.hpp>

GameEngine::GameEngine() : mWindowSize{1280, 1024}, mCurrentScene{SceneId::Play}, mRunning{true}
{
    mSceneMap[SceneId::Play] = std::make_unique<ScenePlay>(*this);
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

Scene* GameEngine::currentScene()
{
    return mSceneMap[mCurrentScene].get();
}

void GameEngine::update()
{
}

void GameEngine::sUserInput()
{
    sf::Event event;

    while (mRenderWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) mRunning = false;

        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            if (currentScene()->getKbdActionMap().contains(event.key.code))
            {
                const auto actionType = currentScene()->getKbdActionMap().at(event.key.code);
                currentScene()->sDoAction(Action(actionType, (event.type == sf::Event::KeyPressed) ? InputEventType::Pressed : InputEventType::Released));
            }
        }
    }
}

void GameEngine::sRender()
{
}
