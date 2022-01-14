#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>

struct TextureSheet
{
    sf::Texture texture;
    uint8_t frameCount;
};

class Animation
{
public:
    Animation(const TextureSheet& sheet, const uint8_t startFrame, const uint8_t frameCount, const uint8_t speed);

    void update();
    bool hasEnded() const;
    sf::Sprite& getSprite();
    sf::Vector2u getSize() const;
    void flipHorizontally();

private:
    sf::Sprite mSprite;
    const sf::Vector2u mSize;
    const uint8_t mStartFrame;
    const uint8_t mFrameCount;
    uint8_t mCurrentFrame;
    uint8_t mSpeed;
};

