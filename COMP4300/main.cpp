#include <iostream>

#include <SFML/Graphics.hpp>

class Vec2
{
public:
    float x = 0;
    float y = 0;

    Vec2() {}
    Vec2(const float xin, const float yin)
        : x(xin), y(yin) {}

    Vec2 operator+(const Vec2 &rhs) { return Vec2{ x + rhs.x, y + rhs.y }; }
    Vec2 operator*(const float scale) { return Vec2{ x * scale, y * scale }; }
    bool operator==(const Vec2& rhs) { return rhs.x == x && rhs.y == y; }

    Vec2& scale(const float scale) { x *= scale; y *= scale; return *this; }
    Vec2& add(const float val) { x += val; y += val; return *this; }

    float dist(const Vec2& rhs) { return 0; }
};

std::ostream& operator<<(std::ostream& os, const Vec2 &vec) { os << vec.x << " " << vec.y; return os; }

Vec2 operator*(const float scale, const Vec2& vec) { return Vec2{ vec.x * scale, vec.y * scale }; }

int main()
{
    Vec2 p1{ 100, 200 };
    Vec2 p2{ 100, 200 };

    auto p3 = 2.0 * p2;
    std::cout << p3;

    return 0;

    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}