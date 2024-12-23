#pragma once

#include "Animation.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>

namespace goldenhand
{
    class Component
    {
    public:
        Component() : has{ false } {}
        ~Component() = default;

        bool has;
    };
} // namespace goldenhand


class CTransform : public goldenhand::Component
{
public:
    sf::Vector2f pos; // indicates the center of the entity
    sf::Vector2f prevPos; // indicates the entity's position on the previous frame
    sf::Vector2f velocity;
    float angle;

    CTransform() = default;
    CTransform(const sf::Vector2f& p, const sf::Vector2f& v, const float a)
        : pos{ p }, prevPos{ p }, velocity{ v }, angle{ a } {}

    void setPos(const sf::Vector2f newPos)
    {
        prevPos = pos;
        pos = newPos;
    }

    void setPosX(const float newX)
    {
        prevPos.x = pos.x;
        pos.x = newX;
    }

    void setPosY(const float newY)
    {
        prevPos.y = pos.y;
        pos.y = newY;
    }
};

class CAnimation : public goldenhand::Component
{
public:
    goldenhand::Animation anim;
    
    CAnimation() = default;
    CAnimation(const goldenhand::Animation& a) : anim{ a } {}
};

class CBoundingBox : public goldenhand::Component
{
public:
    sf::Vector2f size;
    sf::Vector2f halfSize;

    CBoundingBox() = default;
    CBoundingBox(const sf::Vector2f& s) : size{ s }, halfSize{ s.x / 2, s.y / 2 } {}
    CBoundingBox(const sf::Vector2u& s) : CBoundingBox{ sf::Vector2f{ static_cast<float>(s.x), static_cast<float>(s.y) } } {}
};

class CScore : public goldenhand::Component
{
public:
    int score;

    CScore() = default;
    CScore(const int s) : score(s) {}
};

class CLifeSpan : public goldenhand::Component
{
public:
    int total;
    int remaining;

    CLifeSpan() = default;
    CLifeSpan(const int t) : remaining{ t }, total{ t } {}
};

class CDraggable : public goldenhand::Component
{
public:
    bool dragging;

    CDraggable() : dragging{ false } {}
};

class CGravity : public goldenhand::Component {};

class CCooldown : public goldenhand::Component
{
public:
    int total;
    int remaining;

    CCooldown() = default;
    CCooldown(const int t) : total{ t }, remaining { 0 } {}
};