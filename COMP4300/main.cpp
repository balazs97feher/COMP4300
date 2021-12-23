#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Vector2.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

class BouncingShape
{
public:
    BouncingShape(std::unique_ptr<sf::Shape> &&shape, const sf::Vector2f velocity, const sf::Text& text)
        : mShape(std::move(shape)), mVelocity(velocity), mText(text)
    {
    }

    void update(sf::RenderWindow &window)
    {
        const auto wSize = window.getSize();
        const auto sSize = mShape->getLocalBounds();
        const auto sPos = mShape->getPosition();

        if (sPos.x <= 0 || sPos.x + sSize.width >= wSize.x) mVelocity.x *= -1;
        if (sPos.y <= 0 || sPos.y + sSize.height >= wSize.y) mVelocity.y *= -1;

        mText.move(mVelocity);
        mShape->move(mVelocity);

        window.draw(*mShape);
        window.draw(mText);
    }

private:
    std::unique_ptr<sf::Shape> mShape;
    sf::Vector2f mVelocity;
    sf::Text mText;
};

class ShapeFactory
{
public:
    ShapeFactory(const sf::Font font, const sf::Color color) : mFont(font), mColor(color) {}

    BouncingShape makeCircle(const std::string& text, const float radius, const sf::Color& color, const sf::Vector2f& velocity, const sf::Vector2f& startingPos)
    {
        sf::Text shapeText{ text, mFont };
        shapeText.setFillColor(mColor);
        const auto localBounds = shapeText.getLocalBounds();
        const float textWidth = localBounds.width;
        const float textHeight = localBounds.height;
        shapeText.setPosition({ startingPos.x + (2 * radius - textWidth) / 2.0f, startingPos.y + (2 * radius - textHeight) / 2.0f });

        std::unique_ptr<sf::Shape> shape = std::make_unique<sf::CircleShape>(radius);
        shape->setFillColor(color);
        shape->setPosition(startingPos);

        const auto shapeBounds = shape->getGlobalBounds();

        return BouncingShape{ std::move(shape), velocity, shapeText };
    }

    BouncingShape makeRectangle(const std::string& text, const sf::Vector2f& size, const sf::Color& color, const sf::Vector2f& velocity, const sf::Vector2f& startingPos)
    {
        sf::Text shapeText{ text, mFont };
        shapeText.setFillColor(mColor);
        const auto localBounds = shapeText.getLocalBounds();
        const float textWidth = localBounds.width;
        const float textHeight = localBounds.height;
        shapeText.setPosition({ startingPos.x + (size.x - textWidth) / 2.0f, startingPos.y + (size.y - textHeight) / 2.0f });

        std::unique_ptr<sf::Shape> shape = std::make_unique<sf::RectangleShape>(size);
        shape->setFillColor(color);
        shape->setPosition(startingPos);

        return BouncingShape{ std::move(shape), velocity, shapeText };
    }

private:
    sf::Font mFont;
    sf::Color mColor;
};


int main()
{
    const fs::path configFile{ "./assignment01_config.txt" };
    if (!fs::exists(configFile))
    {
        std::cerr << "Configuration file " << configFile.string() << " does not exit." << std::endl;
        exit(-1);
    }

    std::fstream configuration{ configFile, std::ios::in };
    if (!configuration.is_open())
    {
        std::cerr << "Failed to open configuration file." << std::endl;
        exit(-1);
    }

    std::string nextLine;
    std::getline(configuration, nextLine);
    
    std::stringstream stream{ nextLine };

    int wWidth, wHeight;
    stream >> wWidth >> wHeight;

    std::getline(configuration, nextLine);
    stream = std::stringstream{ nextLine };

    std::string fontName;
    stream >> fontName;

    sf::Font font;
    if (!font.loadFromFile("./" + fontName))
    {
        std::cerr << "Failed to load font " << fontName << std::endl;
        exit(-1);
    }

    int textSize;
    stream >> textSize;

    sf::Color textColor;
    stream >> textColor.r >> textColor.g >> textColor.g;

    std::vector<BouncingShape> shapes;
    ShapeFactory shapeFactory{ font, textColor };

    std::getline(configuration, nextLine);
    while(!configuration.eof())
    {
        stream = std::stringstream{ nextLine };

        std::string shape, text;
        stream >> shape >> text;

        sf::Vector2f startingPos, velocity;
        stream >> startingPos.x >> startingPos.y >> velocity.x >> velocity.y;
        int r, g, b;
        stream >> r >> g >> b;
        sf::Color shapeColor(r, g, b);

        if (shape == "Circle")
        {
            float radius;
            stream >> radius;

            shapes.push_back(shapeFactory.makeCircle(text, radius, shapeColor, velocity, startingPos));
        }
        else
        {
            sf::Vector2f rectangleSize;
            stream >> rectangleSize.x >> rectangleSize.y;

            shapes.push_back(shapeFactory.makeRectangle(text, rectangleSize, shapeColor, velocity, startingPos));
        }

        std::getline(configuration, nextLine);
    }


    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "Bouncing Shapes");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        for (auto& shape : shapes)
        {
            shape.update(window);
        }

        window.display();
    }

    return 0;
}