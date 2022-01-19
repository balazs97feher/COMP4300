#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <optional>

class Physics
{
public:
    Physics(const sf::Vector2f& gravity = { 0, 0 });

    /*
    * Return the rectangle of the intersection defined by its top-left corner and size
    */
    static std::optional<sf::FloatRect> boxesOverlap(const sf::Vector2f& oneCenterPos, const sf::Vector2f& oneHalfSize,
        const sf::Vector2f& otherCenterPos, const sf::Vector2f& otherHalfSize);

    static std::optional<sf::Vector2f> lineSegmentsIntersect(const sf::Vector2f& startL1, const sf::Vector2f& endL1,
        const sf::Vector2f& startL2, const sf::Vector2f& endL2);

    static float crossProduct(const sf::Vector2f & one, const sf::Vector2f & other);

private:
    sf::Vector2f mGravity;
};

