#pragma once

#include "Components.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>
#include <string_view>

namespace goldenhand
{
    struct TextureSheet
    {
        sf::Texture texture;
        uint8_t frameCount; // specifies the total number of frames in the texture
    };

    class Animation : public Component
    {
    public:
        Animation() = default;
        Animation(const std::string_view name, const TextureSheet& sheet, const uint8_t startFrame, const uint8_t frameCount, const uint8_t speed);
        Animation(const Animation& a);
        Animation& operator=(const Animation& a);

        void reset();
        void setLoop(const bool playInLoop);

        void update();
        /*
        * Returns true when all the animation frames have been played. Note that it returns false when the animation is created 
        */
        bool hasEnded() const;
        sf::Sprite& getSprite();
        sf::Vector2u getSize() const;
        std::string_view getName() const;

    private:
        std::string_view mName;
        sf::Sprite mSprite;
        sf::Vector2u mSize;
        uint8_t mStartFrame; // specifies the first frame on the texture that belongs to this specific animation
        uint8_t mFrameCount; // specifies the number of frames that belong to this specific animation on the corresponding texture
        uint32_t mCurrentFrame;
        uint8_t mSpeed;
        uint32_t mFrameCounter;
        bool mLoop;

        void copySprite(const sf::Sprite& src, sf::Sprite& dest);
    };
} // namespace goldenhand
