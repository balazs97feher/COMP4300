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
    std::optional<sf::FloatRect> boxesOverlap(const sf::Vector2f& oneCenterPos, const sf::Vector2f& oneHalfSize,
        const sf::Vector2f& otherCenterPos, const sf::Vector2f& otherHalfSize) const;

private:
    sf::Vector2f mGravity;
};

