#include "Physics.h"

#include <cmath>

using namespace std;

Physics::Physics(const sf::Vector2f& gravity) : mGravity{ gravity }
{
}

optional<sf::FloatRect> Physics::boxesOverlap(const sf::Vector2f& oneCenterPos, const sf::Vector2f& oneHalfSize,
    const sf::Vector2f& otherCenterPos, const sf::Vector2f& otherHalfSize) const
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
