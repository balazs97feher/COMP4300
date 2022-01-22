#include "Constants.h"
#include "GameEngine.h"
#include "SceneAnimation.h"

#include <SFML/Window/Keyboard.hpp>

SceneAnimation::SceneAnimation(goldenhand::GameEngine& engine) : Scene{engine}, mAssetManager{ "./config/" }
{
    using namespace goldenhand;

    registerAction(sf::Keyboard::A, ActionType::MoveLeft);
    registerAction(sf::Keyboard::D, ActionType::MoveRight);
    registerAction(sf::Keyboard::Escape, ActionType::Quit);

    initialize();
}

void SceneAnimation::initialize()
{
    mAssetManager.loadAssets();

    spawnPlayer();
}

void SceneAnimation::update()
{
    if (!mHasEnded)
    {
        mEntityManager.update();

        sMovement();
        sAnimation();
        sRender();

        mCurrentFrame++;
    }
}

void SceneAnimation::sDoAction(const goldenhand::Action action)
{
    using namespace goldenhand;

    switch (action.getType())
    {
        case ActionType::Quit:
            if (action.getEventType() == InputEventType::Released) mEngine.changeScene(goldenhand::SceneId::Menu);
            return;
        case ActionType::MoveLeft:
            if (action.getEventType() == InputEventType::Pressed) mPlayer->getComponent<CTransform>().velocity = sf::Vector2f{ -5, 0 };
            else mPlayer->getComponent<CTransform>().velocity = sf::Vector2f{ 0, 0 };
            break;
        case ActionType::MoveRight:
            if (action.getEventType() == InputEventType::Pressed) mPlayer->getComponent<CTransform>().velocity = sf::Vector2f{ 5, 0 };
            else mPlayer->getComponent<CTransform>().velocity = sf::Vector2f{ 0, 0 };
            break;
        default:
            break;
    }
}

void SceneAnimation::sRender()
{
    auto sprite = mAssetManager.getAnimation(mPlayer->getComponent<CAnimation>().animation).getSprite();
    sprite.setPosition(mPlayer->getComponent<CTransform>().pos);
    mEngine.drawToWindow(sprite);
}

void SceneAnimation::sMovement()
{
    mPlayer->getComponent<CTransform>().prevPos = mPlayer->getComponent<CTransform>().pos;
    mPlayer->getComponent<CTransform>().pos += mPlayer->getComponent<CTransform>().velocity;

    if (mPlayer->getComponent<CTransform>().pos.x > mPlayer->getComponent<CTransform>().prevPos.x)
    {
        mPlayer->getComponent<CTransform>().angle = 1;
    }
    else if (mPlayer->getComponent<CTransform>().pos.x < mPlayer->getComponent<CTransform>().prevPos.x)
    {
        mPlayer->getComponent<CTransform>().angle = -1;
    }
}

void SceneAnimation::sAnimation()
{
    if (mPlayer->getComponent<CTransform>().velocity.x == 0) mPlayer->getComponent<CAnimation>().animation = Constants::Animation::megaman_standing;
    else mPlayer->getComponent<CAnimation>().animation = Constants::Animation::megaman_running;

    auto& animation = mAssetManager.getAnimation(mPlayer->getComponent<CAnimation>().animation);

    if (mPlayer->getComponent<CTransform>().angle == 1) animation.getSprite().setScale(2.0f, 2.0f);
    else animation.getSprite().setScale(-2.0f, 2.0f);

    animation.update();
}

void SceneAnimation::spawnPlayer()
{
    mPlayer = mEntityManager.addEntity(goldenhand::EntityTag::Player);

    mPlayer->addComponent<CTransform>(sf::Vector2f{ 200, 400 }, sf::Vector2f{ 0, 0 }, 1.0f);
    mPlayer->addComponent<CAnimation>(Constants::Animation::megaman_standing);
}
