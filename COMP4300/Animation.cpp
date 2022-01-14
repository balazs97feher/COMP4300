#include "Animation.h"

#include <cassert>

Animation::Animation(const TextureSheet& sheet, const uint8_t startFrame, const uint8_t frameCount, const uint8_t speed)
    : mSprite{ sheet.texture }, mSize{ sheet.texture.getSize().x / sheet.frameCount, sheet.texture.getSize().y },
    mStartFrame{ startFrame }, mFrameCount{ frameCount }, mSpeed{ speed }, mCurrentFrame{ 0 }
{
    assert(frameCount > 0);

    mSprite.setTextureRect(sf::IntRect(mSize.x * startFrame, 0u, mSize.x, mSize.y));
    mSprite.setOrigin(mSize.x / 2.0f, mSize.y / 2.0f);
}

void Animation::update()
{
    mSprite.setTextureRect(sf::IntRect(mSize.x * (mStartFrame + (mSpeed == 0 ? 0 : mCurrentFrame / mSpeed) % mFrameCount), 0u, mSize.x, mSize.y));

    mCurrentFrame++;
}

bool Animation::hasEnded() const
{
    return mCurrentFrame == 0;
}

sf::Sprite& Animation::getSprite()
{
    return mSprite;
}

sf::Vector2u Animation::getSize() const
{
    return mSize;
}

void Animation::flipHorizontally()
{
    mSprite.setScale(-1.0f, 1.0f);
}
