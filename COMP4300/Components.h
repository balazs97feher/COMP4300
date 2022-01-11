#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

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
    sf::Vector2f velocity;
    float angle;

    CTransform() = default;
    CTransform(const sf::Vector2f& p, const sf::Vector2f& v, const float a)
        : pos{ p }, velocity{ v }, angle{ a } {}
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

class CCollision : public Component
{
public:
    float radius;

    CCollision() = default;
    CCollision(const float r) : radius{ r } {}
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
