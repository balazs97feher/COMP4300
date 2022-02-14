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

        /*
        * Returns the intersection point of a line segment and a rectangle
        */
        static std::optional<sf::Vector2f> lineSegmentRectangleIntersect(const sf::Vector2f& startLine, const sf::Vector2f& endLine,
            const sf::Vector2f& rectPos, const sf::Vector2f& rectHalfSize);

        static float crossProduct(const sf::Vector2f & one, const sf::Vector2f & other);

        template<typename T>
        static bool isWithinBounds(const T value, const T lowerBound, const T upperBound)
        {
            return value >= lowerBound && value <= upperBound;
        }

        template<typename T>
        static bool isWithinRectangle(const sf::Vector2<T> point, const sf::Vector2<T> centerPos, const sf::Vector2<T> rectSize)
        {
            return isWithinBounds(point.x, centerPos.x - rectSize.x / 2, centerPos.x + rectSize.x / 2) &&
                isWithinBounds(point.y, centerPos.y - rectSize.y / 2, centerPos.y + rectSize.y / 2);
        }

        sf::Vector2f gravity() const;

    private:
        sf::Vector2f mGravity;
    };
} // namespace goldenhand
