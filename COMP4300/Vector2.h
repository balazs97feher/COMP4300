#pragma once

#include <ostream>

class Vector2
{
public:
    float x = 0;
    float y = 0;

    Vector2() {}
    Vector2(const float xin, const float yin)
        : x(xin), y(yin) {}

    Vector2 operator+(const Vector2& rhs) { return Vector2{ x + rhs.x, y + rhs.y }; }
    Vector2 operator*(const float scale) { return Vector2{ x * scale, y * scale }; }
    bool operator==(const Vector2& rhs) { return rhs.x == x && rhs.y == y; }

    Vector2& scale(const float scale) { x *= scale; y *= scale; return *this; }
    Vector2& add(const float val) { x += val; y += val; return *this; }

    float dist(const Vector2& rhs) { return 0; }
};

std::ostream& operator<<(std::ostream& os, const Vector2& vec);

Vector2 operator*(const float scale, const Vector2& vec);
