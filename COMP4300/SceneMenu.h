#pragma once

#include "Scene.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include <functional>
#include <vector>

class SceneMenu : public Scene
{
public:
    SceneMenu(GameEngine& engine);

    virtual void initialize() override;

    virtual void update() override;
    virtual void sDoAction(const Action action) override;
    virtual void sRender() override;

private:
    class Item
    {
    public:
        Item() = default;
        Item(SceneMenu* menu, const std::string& text, std::function<void(void)> action);

        SceneMenu* mMenu;
        sf::Text mText;
        std::function<void(void)> execute;
    };
    friend class Item;

    std::vector<Item> mItems;
    const int mCharSize = 48;
    const sf::Color mInactiveColor;
    const sf::Color mSelectedColor;
    sf::Font mFont;
    int mSelectedIdx;

    void addItem(const std::string& text, std::function<void(void)> action);
};

