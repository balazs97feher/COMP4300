#include "Animation.h"

#include <cassert>

namespace goldenhand
{
    Animation::Animation(const std::string_view name, const TextureSheet& sheet, const uint8_t startFrame, const uint8_t frameCount, const uint8_t speed)
        : mName{ name }, mSprite{ sheet.texture }, mSize{ sheet.texture.getSize().x / sheet.frameCount, sheet.texture.getSize().y },
        mStartFrame{ startFrame }, mFrameCount{ frameCount }, mSpeed{ speed }, mCurrentFrame{ 0 }, mFrameCounter{ 0 }, mLoop{ true }
    {
        assert(frameCount > 0);

        mSprite.setTextureRect(sf::IntRect(mSize.x * startFrame, 0u, mSize.x, mSize.y));
        mSprite.setOrigin(mSize.x / 2.0f, mSize.y / 2.0f);
    }

    Animation::Animation(const Animation& a)
        : mName {a.mName}, mSize{ a.mSize }, mStartFrame{ a.mStartFrame }, mFrameCount{ a.mFrameCount },
        mSpeed{ a.mSpeed }, mCurrentFrame{ a.mCurrentFrame }, mFrameCounter{ 0 }, mLoop{ a.mLoop }
    {
        copySprite(a.mSprite, mSprite);
    }

    Animation& Animation::operator=(const Animation& a)
    {
        mName = a.mName;
        copySprite(a.mSprite, mSprite);
        mSize = a.mSize;
        mStartFrame = a.mStartFrame;
        mFrameCount = a.mFrameCount;
        mSpeed = a.mSpeed;
        mCurrentFrame = a.mCurrentFrame;
        mFrameCounter = 0;
        mLoop = a.mLoop;
        return *this;
    }

    void Animation::reset()
    {
        mCurrentFrame = 0;
        mFrameCounter = 0;
    }

    void Animation::setLoop(const bool playInLoop)
    {
        mLoop = playInLoop;
    }

    void Animation::update()
    {
        if (!mLoop && hasEnded()) return;

        mCurrentFrame = (mSpeed == 0 ? 0 : mFrameCounter / mSpeed) % mFrameCount;
        mSprite.setTextureRect(sf::IntRect(mSize.x * (mStartFrame + mCurrentFrame), 0u, mSize.x, mSize.y));

        mFrameCounter++;
    }

    bool Animation::hasEnded() const
    {
        return !mLoop && (mCurrentFrame == mFrameCount - 1);
    }

    sf::Sprite& Animation::getSprite()
    {
        return mSprite;
    }

    sf::Vector2u Animation::getSize() const
    {
        return mSize;
    }

    std::string_view Animation::getName() const
    {
        return mName;
    }

    void Animation::copySprite(const sf::Sprite& src, sf::Sprite& dest)
    {
        dest.setTexture(*src.getTexture());
        dest.setTextureRect(src.getTextureRect());
        dest.setOrigin(src.getOrigin());
    }
} // namespace goldenhand
