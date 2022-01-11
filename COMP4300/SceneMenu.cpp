#include "SceneMenu.h"
#include "GameEngine.h"

#include <SFML/Window.hpp>

#include <iostream>

using namespace std;

SceneMenu::SceneMenu(GameEngine& engine) : Scene{engine}, mInactiveColor{255, 0, 0}, mSelectedColor{0, 255, 0}, mSelectedIdx{0}
{
    registerAction(sf::Keyboard::Up, ActionType::MoveUp);
    registerAction(sf::Keyboard::Down, ActionType::MoveDown);
    registerAction(sf::Keyboard::Enter, ActionType::Select);

    initialize();
}

void SceneMenu::initialize()
{
    if (!mFont.loadFromFile("./config/fonts/ayar.ttf"))
    {
        cerr << "Failed to load font: ./config/ayar.ttf" << endl;
        exit(-1);
    }

    mContinueText.setFont(mFont);
    mContinueText.setCharacterSize(mCharSize);
    mContinueText.setFillColor(mInactiveColor);
    mContinueText.setString("CONTINUE");
    mContinueText.setPosition({ (mEngine.windowSize().x / 2.0f) - (mContinueText.getLocalBounds().width / 2), mEngine.windowSize().y / 3.0f });
    mRestartText.setFont(mFont);
    mRestartText.setCharacterSize(mCharSize);
    mRestartText.setFillColor(mInactiveColor);
    mRestartText.setString("RESTART");
    mRestartText.setPosition({ (mEngine.windowSize().x / 2.0f) - (mRestartText.getLocalBounds().width / 2), mEngine.windowSize().y * 2 / 3.0f });
}

void SceneMenu::update()
{
}

void SceneMenu::sDoAction(const Action action)
{
}

void SceneMenu::sRender()
{
    mEngine.drawToWindow(mContinueText);
    mEngine.drawToWindow(mRestartText);
}
