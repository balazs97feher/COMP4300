#include "Physics.h"

#include <cmath>

using namespace std;

namespace goldenhand
{
    Physics::Physics(const sf::Vector2f& gravity) : mGravity{ gravity }
    {
    }

    optional<sf::FloatRect> Physics::boxesOverlap(const sf::Vector2f& oneCenterPos, const sf::Vector2f& oneHalfSize,
        const sf::Vector2f& otherCenterPos, const sf::Vector2f& otherHalfSize)
    {
        const auto distVec = oneCenterPos - otherCenterPos;
        const auto dx = abs(distVec.x);
        const auto dy = abs(distVec.y);

        const auto overlap = sf::Vector2f{ oneHalfSize.x + otherHalfSize.y - dx,
            oneHalfSize.y + otherHalfSize.y - dy };

        if (overlap.x > 0 && overlap.y > 0) return sf::FloatRect{ min(oneCenterPos.x + oneHalfSize.x, otherCenterPos.x + otherHalfSize.x) - overlap.x,
            max(oneCenterPos.y - oneHalfSize.y, otherCenterPos.y - otherHalfSize.y), overlap.x, overlap.y };

        return nullopt;
    }

    std::optional<sf::Vector2f> Physics::lineSegmentsIntersect(const sf::Vector2f& startL1, const sf::Vector2f& endL1,
        const sf::Vector2f& startL2, const sf::Vector2f& endL2)
    {
        const auto L1 = endL1 - startL1;
        const auto L2 = endL2 - startL2;

        const auto L1xL2 = crossProduct(L1, L2);
        const auto startL1L2 = startL2 - startL1;
        const auto r = crossProduct(startL1L2, L2) / L1xL2;
        const auto t = crossProduct(startL1L2, L1) / L1xL2;

        if (t >= 0 && t <= 1 && r >= 0 && r <= 1) return startL1 + (L1 * r);

        return nullopt;
    }

    float Physics::crossProduct(const sf::Vector2f& one, const sf::Vector2f& other)
    {
        return one.x * other.y - other.x * one.y;
    }

    sf::Vector2f Physics::gravity() const
    {
        return mGravity;
    }
} // namespace goldenhand
