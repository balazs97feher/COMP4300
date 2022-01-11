#include "ScenePlay.h"

#include <SFML/Window.hpp>

ScenePlay::ScenePlay(GameEngine& engine) : Scene{engine}
{
    registerAction(sf::Keyboard::W, ActionType::MoveUp);
    registerAction(sf::Keyboard::A, ActionType::MoveLeft);
    registerAction(sf::Keyboard::S, ActionType::MoveDown);
    registerAction(sf::Keyboard::D, ActionType::MoveRight);
    registerAction(sf::Keyboard::Space, ActionType::Shoot);
    registerAction(sf::Keyboard::Escape, ActionType::Quit);
}

void ScenePlay::initialize()
{
    // todo: config file reading
}

void ScenePlay::update()
{
    while (!mHasEnded)
    {
        if (!mPaused)
        {
            mEntityManager.update();

            sMovement();
            sCollision();
            sEnemySpawner();
            sAttack();
            sLifespan();

            mCurrentFrame++;
        }
    }
}

void ScenePlay::sDoAction(const Action action)
{
}

void ScenePlay::sRender()
{
}

void ScenePlay::sMovement()
{
}

void ScenePlay::sLifespan()
{
}

void ScenePlay::sAttack()
{
}

void ScenePlay::sEnemySpawner()
{
}

void ScenePlay::sCollision()
{
}
