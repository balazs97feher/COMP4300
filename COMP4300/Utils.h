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
}