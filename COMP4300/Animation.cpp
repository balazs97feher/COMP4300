#include "Animation.h"

Animation::Animation(const TextureSheet& sheet, const uint8_t startFrame, const uint8_t frameCount, const uint8_t speed)
    : mSprite{ sheet.texture }, mSize{ sheet.texture.getSize().x / sheet.frameCount, sheet.texture.getSize().y },
    mStartFrame{ startFrame }, mFrameCount{ frameCount }, mSpeed{ speed }, mCurrentFrame{ 0 }
{
    mSprite.setTextureRect(sf::IntRect(mSize.x * startFrame, 0u, mSize.x, mSize.y));
    mSprite.setOrigin(mSize.x / 2.0f, mSize.y / 2.0f);
}

void Animation::update()
{
    mCurrentFrame = (++mCurrentFrame) % mFrameCount;
    mSprite.setTextureRect(sf::IntRect(mSize.x * (mStartFrame + mCurrentFrame), 0u, mSize.x, mSize.y));
}

bool Animation::hasEnded() const
{
    return mCurrentFrame == 0;
}

sf::Vector2u Animation::getSize() const
{
    return mSize;
}

void Animation::flipHorizontally()
{
    mSprite.setScale(-1.0f, 1.0f);
}
