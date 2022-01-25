#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <optional>

namespace goldenhand
{
    class Physics
    {
    public:
        Physics(const sf::Vector2f& gravity = { 0, 0 });

        /*
        * Returns the rectangle of the intersection defined by its top-left corner and size
        */
        static std::optional<sf::FloatRect> boxesOverlap(const sf::Vector2f& oneCenterPos, const sf::Vector2f& oneHalfSize,
            const sf::Vector2f& otherCenterPos, const sf::Vector2f& otherHalfSize);

        /*
        * Returns the magnitude of overlap in the x and y directions
        */
        static sf::Vector2f boxesDimensionalOverlap(const sf::Vector2f& oneCenterPos, const sf::Vector2f& oneHalfSize,
            const sf::Vector2f& otherCenterPos, const sf::Vector2f& otherHalfSize);

        /*
        * Returns the intersection point of the two line segments
        */
        static std::optional<sf::Vector2f> lineSegmentsIntersect(const sf::Vector2f& startL1, const sf::Vector2f& endL1,
            const sf::Vector2f& startL2, const sf::Vector2f& endL2);

        static float crossProduct(const sf::Vector2f & one, const sf::Vector2f & other);

        sf::Vector2f gravity() const;

    private:
        sf::Vector2f mGravity;
    };
} // namespace goldenhand
