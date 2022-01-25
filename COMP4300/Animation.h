#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>

namespace goldenhand
{
    struct TextureSheet
    {
        sf::Texture texture;
        uint8_t frameCount;
    };

    class Animation
    {
    public:
        Animation(const TextureSheet& sheet, const uint8_t startFrame, const uint8_t frameCount, const uint8_t speed);

        void reset();
        void setLoop(const bool playInLoop);

        void update();
        /*
        * returns true when all the animation frames have been played. Note that it returns false when the animation is created 
        */
        bool hasEnded() const;
        sf::Sprite& getSprite();
        sf::Vector2u getSize() const;

    private:
        sf::Sprite mSprite;
        const sf::Vector2u mSize;
        const uint8_t mStartFrame;
        const uint8_t mFrameCount;
        uint32_t mCurrentFrame;
        uint8_t mSpeed;
        uint32_t mFrameCounter;
        bool mLoop;
    };
} // namespace goldenhand
