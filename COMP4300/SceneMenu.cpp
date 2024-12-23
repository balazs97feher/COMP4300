#include "Constants.h"
#include "SceneMenu.h"
#include "GameEngine.h"

#include <SFML/Window.hpp>

#include <iostream>

using namespace std;

SceneMenu::SceneMenu(goldenhand::GameEngine& engine) : Scene{engine}, mInactiveColor{255, 0, 0}, mSelectedColor{0, 255, 0}, mSelectedIdx{0}
{
    using namespace goldenhand;

    registerKbdAction(sf::Keyboard::Up, ActionType::MoveUp);
    registerKbdAction(sf::Keyboard::Down, ActionType::MoveDown);
    registerKbdAction(sf::Keyboard::Enter, ActionType::Select);
    registerKbdAction(sf::Keyboard::Escape, ActionType::Quit);

    initialize();
}

void SceneMenu::initialize()
{
    if (!mFont.loadFromFile("./config/fonts/ayar.ttf"))
    {
        cerr << "Failed to load font: ./config/ayar.ttf" << endl;
        exit(-1);
    }

    auto startPlatform = [this]() {
        this->mEngine.createScene(Constants::Scene::platform);
    };
    addItem("START", startPlatform);

    auto continuePlatform = [this]() {
        this->mEngine.changeScene(Constants::Scene::platform);
    };
    addItem("CONTINUE", continuePlatform);
}

void SceneMenu::update()
{
    if (!mHasEnded)
    {
        const sf::Vector2f wSize{ static_cast<float>(mEngine.windowSize().x), static_cast<float>(mEngine.windowSize().y) };
        mEngine.setView(sf::View( wSize / 2.f , wSize));

        for (auto& item : mItems) item.mText.setFillColor(mInactiveColor);
        mItems[mSelectedIdx].mText.setFillColor(mSelectedColor);

        mCurrentFrame++;
    }
}

void SceneMenu::sDoAction(const goldenhand::Action action)
{
    using namespace goldenhand;

    if (action.getEventType() != InputEventType::Released) return;

    switch (action.getType())
    {
        case ActionType::MoveDown:
            mSelectedIdx = (mSelectedIdx + 1) % mItems.size();
            break;
        case ActionType::MoveUp:
            mSelectedIdx = (mSelectedIdx - 1) % mItems.size();
            break;
        case ActionType::Select:
            mItems[mSelectedIdx].execute();
            break;
        case ActionType::Quit:
            mHasEnded = true;
            mEngine.quit();
            break;
        default:
            break;
    }
}

void SceneMenu::sRender()
{
    for (const auto& item : mItems)
    {
        mEngine.drawToWindow(item.mText);
    }
}

void SceneMenu::addItem(const std::string& text, std::function<void(void)> action)
{
    mItems.emplace_back(this, text, action);
    
    for (size_t i = 0; i < mItems.size(); i++)
    {
        mItems[i].mText.setPosition({ (mEngine.windowSize().x / 2.0f) - (mItems[i].mText.getLocalBounds().width / 2),
            mEngine.windowSize().y * (i + 1) / (mItems.size() + 1.0f) });
    }
}

SceneMenu::Item::Item(SceneMenu* menu, const std::string& text, std::function<void(void)> action)
    : mMenu{menu}, execute{action}
{
    mText.setString(text);
    mText.setFillColor(mMenu->mInactiveColor);
    mText.setCharacterSize(mMenu->mCharSize);
    mText.setFont(mMenu->mFont);
}
