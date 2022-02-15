#pragma once

#include <algorithm>
#include <concepts>
#include <numbers>
#include <type_traits>

#include <SFML/System/Vector2.hpp>

constexpr inline float pi = static_cast<float>(std::numbers::pi);

namespace goldenhand
{
    template<std::floating_point F>
    bool equal(const F left, const F right)
    {
        static constexpr F epsilon = 1.0e-05;
        
        if (abs(left - right) <= epsilon) return true;

        return abs(left - right) <= epsilon * std::max(abs(left), abs(right));
    }

    template<typename T>
    T dotProduct(const sf::Vector2<T> left, const sf::Vector2<T> right)
    {
        return left.x * right.x + left.y * right.y;
    }

    template<typename T>
    float length(const sf::Vector2<T> vec)
    {
        return static_cast<float>(sqrt(dotProduct(vec, vec)));
    }

    template<typename T>
    float angle(const sf::Vector2<T> one, const sf::Vector2<T> other)
    {
        return acosf(static_cast<float>(dotProduct(one, other) / (length(one) * length(other))));
    }

    sf::Vector2f dirVector(const float angle)
    {
        return { cos(angle), sin(angle) };
    }
}