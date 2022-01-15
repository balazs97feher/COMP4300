#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>

class Component
{
public:
    Component() : has{ false } {}
    ~Component() = default;

    bool has;
};

class CTransform : public Component
{
public:
    sf::Vector2f pos; // indicates the center of the entity
    sf::Vector2f prevPos; // indicates the entity's position on the previous frame
    sf::Vector2f velocity;
    float angle;

    CTransform() = default;
    CTransform(const sf::Vector2f& p, const sf::Vector2f& v, const float a)
        : pos{ p }, prevPos{ p }, velocity{ v }, angle{ a } {}
};

class CAnimation : public Component
{
public:
    std::string animation;

    CAnimation() = default;
    CAnimation(const std::string& a) : animation{ a } {}
};

class CShape : public Component
{
public:
    sf::CircleShape circle;

    CShape() = default;
    CShape(const float radius, const int points, const sf::Color& fill, const sf::Color& outline, const float thickness)
        : circle{ radius, points }
    {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(radius, radius); // defines the center point for all transformations
    }
};

class CBoundingBox : public Component
{
public:
    sf::Vector2f size;
    sf::Vector2f halfSize;
    sf::RectangleShape rect;

    CBoundingBox() = default;
    CBoundingBox(const sf::Vector2f& s) : size{ s }, halfSize{ s.x / 2, s.y / 2 }, rect{ size }
    {
        rect.setOrigin(halfSize);
        rect.setFillColor(sf::Color{ 255, 255, 255, 0 });
        rect.setOutlineColor(sf::Color{ 255, 255, 255 });
        rect.setOutlineThickness(1);
    }
};

class CScore : public Component
{
public:
    int score;

    CScore() = default;
    CScore(const int s) : score(s) {}
};

class CLifeSpan : public Component
{
public:
    int total;
    int remaining;

    CLifeSpan() = default;
    CLifeSpan(const int t) : remaining{ t }, total{ t } {}
};
