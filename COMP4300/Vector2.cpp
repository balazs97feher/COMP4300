#include "Vector2.h"

std::ostream& operator<<(std::ostream& os, const Vector2& vec)
{
    os << vec.x << " " << vec.y;
    return os;
}

Vector2 operator*(const float scale, const Vector2& vec)
{
    return Vector2{ vec.x * scale, vec.y * scale };
}
