#include "Animation.h"

Animation::Animation(const TextureSheet& sheet, const uint8_t startFrame, const uint8_t frameCount, const uint8_t speed)
    : mSprite{ sheet.texture }, mStartFrame{ startFrame }, mFrameCount{ frameCount }, mSpeed{ speed }, mCurrentFrame{ 0 }
{
    mSize = sf::Vector2u{ sheet.texture.getSize().x / sheet.frameCount, sheet.texture.getSize().y };

    mSprite.setTextureRect(sf::IntRect(mSize.x * startFrame, 0u, mSize.x, mSize.y));
}
