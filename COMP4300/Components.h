#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

class CTransform
{
public:
    sf::Vector2f pos; // indicates the center of the entity
    sf::Vector2f velocity;
    float angle;

    CTransform(const sf::Vector2f& p, const sf::Vector2f& v, const float a)
        : pos{ p }, velocity{ v }, angle{ a } {}
};

class CShape
{
public:
    sf::CircleShape circle;

    CShape(const float radius, const int points, const sf::Color& fill, const sf::Color& outline, const float thickness)
        : circle{ radius, points }
    {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(radius, radius); // defines the center point for all transformations
    }
};

class CCollision
{
public:
    float radius;

    CCollision(const float r) : radius{ r } {}
};

class CScore
{
public:
    int score;

    CScore(const int s) : score(s) {}
};

class CLifeSpan
{
public:
    int remaining;
    const int total;

    CLifeSpan(const int t) : remaining{ t }, total{ t } {}
};

class CInput
{
public:
    bool up, left, right, down, shoot;

    CInput() : up{ false }, left{ false }, right{ false }, down{ false }, shoot{ false } {}
};