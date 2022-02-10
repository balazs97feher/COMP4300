#include "ConfigReader.h"
#include "Constants.h"
#include "GameEngine.h"
#include "ScenePlatform.h"

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;

ScenePlatform::ScenePlatform(goldenhand::GameEngine& engine)
    : Scene{ engine }, mAssetManager{ "./config/" }, mPhysics({ 0.f, .2f }), mDrawBB{ false }, mTextureDraw{ true }, mCloneSelected{ false }
{
    using namespace goldenhand;

    mBB.setFillColor(sf::Color{ 255, 255, 255, 0 });
    mBB.setOutlineColor(sf::Color{ 255, 255, 255 });
    mBB.setOutlineThickness(1);

    registerKbdAction(sf::Keyboard::A, ActionType::MoveLeft);
    registerKbdAction(sf::Keyboard::D, ActionType::MoveRight);
    registerKbdAction(sf::Keyboard::W, ActionType::MoveUp);
    registerKbdAction(sf::Keyboard::Space, ActionType::Shoot);
    registerKbdAction(sf::Keyboard::Escape, ActionType::Quit);
    registerKbdAction(sf::Keyboard::B, ActionType::ToggleBBDraw);
    registerKbdAction(sf::Keyboard::T, ActionType::ToggleTextureDraw);
    registerKbdAction(sf::Keyboard::C, ActionType::Clone);
    registerKbdAction(sf::Keyboard::L, ActionType::Save);

    registerMouseAction(sf::Mouse::Button::Left, ActionType::Select);

    initialize();
}

void ScenePlatform::initialize()
{
    mAssetManager.loadAssets();

    goldenhand::ConfigReader configuration(mLevel);

    string configType;
    configuration >> configType;
    while (!configuration.eof())
    {
        string name;
        float x, y;
        if (configType == "Player")
        {
            configuration >> mPlayerConfig.startPosX >> mPlayerConfig.startPosY >> mPlayerConfig.runSpeed
                >> mPlayerConfig.jumpSpeed >> mPlayerConfig.maxSpeed >> mPlayerConfig.trapViewRatio;
        }
        else if (configType == "Bullet")
        {
            configuration >> mBulletConfig.speed >> mBulletConfig.rotation >> mBulletConfig.lifespan;
        }
        else if (configType == "Background")
        {
            configuration >> name;
            auto background = mEntityManager.addEntity(EntityTag::Background);
            background->addComponent<CTransform>(sf::Vector2f{ mEngine.windowSize().x / 2.f, mEngine.windowSize().y / 2.f }, sf::Vector2f{ 0, 0 }, 0.f);
            background->addComponent<CAnimation>(name);
            auto& animation = mAssetManager.getAnimation(name);
            animation.getSprite().setScale(static_cast<float>(mEngine.windowSize().x) / animation.getSize().x,
                static_cast<float>(mEngine.windowSize().y) / animation.getSize().y);
        }
        else if (configType == "Tile")
        {
            configuration >> name >> x >> y;
            auto tile = mEntityManager.addEntity(EntityTag::Tile);
            tile->addComponent<CTransform>(sf::Vector2f{ x, y }, sf::Vector2f{ 0, 0 }, 0.f);
            tile->addComponent<CAnimation>(name);
            tile->addComponent<CBoundingBox>(mAssetManager.getAnimation(name).getSize());
            tile->addComponent<CDraggable>();
        }

        configuration >> configType;
    }

    spawnPlayer();
    spawnRobot();
}

void ScenePlatform::update()
{
    if (!mHasEnded)
    {
        mEntityManager.update();

        sLifeSpan();
        sView();
        sMovement();
        sPhysics();
        sAnimation();
        sRender();

        mCurrentFrame++;
    }
}

void ScenePlatform::sDoAction(const goldenhand::Action action)
{
    using namespace goldenhand;

    auto& velocity = mPlayer->getComponent<CTransform>().velocity;

    switch (action.getType())
    {
    case ActionType::Quit:
        if (action.getEventType() == InputEventType::Released) mEngine.changeScene(Constants::Scene::menu);
        break;
    case ActionType::Save:
        if (action.getEventType() == InputEventType::Released) saveLevel();
        break;
    case ActionType::ToggleBBDraw:
        if (action.getEventType() == InputEventType::Released) mDrawBB = !mDrawBB;
        break;
    case ActionType::ToggleTextureDraw:
        if (action.getEventType() == InputEventType::Released) mTextureDraw = !mTextureDraw;
        break;
    case ActionType::MoveLeft:
        if (action.getEventType() == InputEventType::Pressed) velocity = { -mPlayerConfig.runSpeed, velocity.y };
        else velocity = sf::Vector2f{ 0.f, velocity.y };
        break;
    case ActionType::MoveRight:
        if (action.getEventType() == InputEventType::Pressed) velocity = { mPlayerConfig.runSpeed, velocity.y };
        else velocity = sf::Vector2f{ 0.f, velocity.y };
        break;
    case ActionType::Shoot:
        if (action.getEventType() == InputEventType::Released)
        {
            shootBlade();
        }
        break;
    case ActionType::MoveUp:
        if (action.getEventType() == InputEventType::Pressed && velocity.y == 0) velocity.y = -mPlayerConfig.jumpSpeed;
        else if (action.getEventType() == InputEventType::Released) velocity.y = 0;
        break;
    case ActionType::Clone:
        if (action.getEventType() == InputEventType::Pressed) mCloneSelected = true;
        else mCloneSelected = false;
        break;
    case ActionType::Select:
        if (action.getEventType() == InputEventType::Pressed && !mDraggedEntity)
        {
            auto selectedEntity = findSelectedEntity(mEngine.mousePos());
            if (selectedEntity)
            {
                mDraggedEntity = mCloneSelected ? mEntityManager.cloneEntity(selectedEntity) : selectedEntity;
                mDraggedEntity->getComponent<CDraggable>().dragging = true;
            }
        }
        else if (action.getEventType() == InputEventType::Released && mDraggedEntity)
        {
            mDraggedEntity->getComponent<CDraggable>().dragging = false;
            mDraggedEntity.reset();
        }
        break;
    default:
        break;
    }
}

void ScenePlatform::sRender()
{
    for (const auto& entity : mEntityManager.getEntities())
    {
        if (mTextureDraw && entity->hasComponent<CAnimation>())
        {
            auto& sprite = mAssetManager.getAnimation(entity->getComponent<CAnimation>().animation).getSprite();
            sprite.setPosition(entity->getComponent<CTransform>().pos);

            if (entity->tag() == EntityTag::Blade)
            {
                sprite.setRotation(entity->getComponent<CTransform>().angle);
            }

            mEngine.drawToWindow(sprite);
        }
        if (mDrawBB && entity->hasComponent<CBoundingBox>())
        {
            mBB.setSize(entity->getComponent<CBoundingBox>().size);
            mBB.setOrigin(entity->getComponent<CBoundingBox>().halfSize);
            mBB.setPosition(entity->getComponent<CTransform>().pos);
            mEngine.drawToWindow(mBB);
        }
    }
}

void ScenePlatform::sView()
{
    const sf::Vector2f wSize{ static_cast<float>(mEngine.windowSize().x), static_cast<float>(mEngine.windowSize().y) };
    const auto& view = mEngine.getView();
    const auto& playerPos = mPlayer->getComponent<CTransform>().pos;

    if (playerPos.x > ((wSize.x - wSize.x * mPlayerConfig.trapViewRatio) / 2.f))
    {
        if (playerPos.x < (view.getCenter().x - wSize.x * mPlayerConfig.trapViewRatio / 2.f))
        {
            mEngine.setView(sf::View{ {playerPos.x + (wSize.x * mPlayerConfig.trapViewRatio / 2.f), wSize.y / 2}, wSize });
        }
        else if(playerPos.x > (view.getCenter().x + wSize.x * mPlayerConfig.trapViewRatio / 2.f))
        {
            mEngine.setView(sf::View{ {playerPos.x - (wSize.x * mPlayerConfig.trapViewRatio / 2.f), wSize.y / 2}, wSize });
        }
    }

    for (auto& background : mEntityManager.getEntities(EntityTag::Background))
    {
        background->getComponent<CTransform>().setPos(mEngine.mapPixelToCoords(sf::Vector2i(wSize.x / 2, wSize.y / 2)));
    }
}

void ScenePlatform::sMovement()
{
    if (mDraggedEntity)
    {
        mDraggedEntity->getComponent<CTransform>().setPos(sf::Vector2f{ static_cast<float>(mEngine.mousePos().x),
            static_cast<float>(mEngine.mousePos().y) });
    }

    for (auto entity : mEntityManager.getEntities())
    {
        if (entity->hasComponent<CDraggable>() && entity->getComponent<CDraggable>().dragging) continue;
        if (entity->tag() == EntityTag::Tile) continue;

        if (entity->hasComponent<CGravity>())
        {
            entity->getComponent<CTransform>().velocity += mPhysics.gravity();
            entity->getComponent<CTransform>().velocity.y = min(entity->getComponent<CTransform>().velocity.y, mPlayerConfig.maxSpeed);
        }

        entity->getComponent<CTransform>().setPos(entity->getComponent<CTransform>().pos + entity->getComponent<CTransform>().velocity);
        
        if (entity->tag() != EntityTag::Blade)
        {
            if (entity->getComponent<CTransform>().pos.x > entity->getComponent<CTransform>().prevPos.x)
            {
                entity->getComponent<CTransform>().angle = 1;
            }
            else if (entity->getComponent<CTransform>().pos.x < entity->getComponent<CTransform>().prevPos.x)
            {
                entity->getComponent<CTransform>().angle = -1;
            }
        }
        else
        {
            entity->getComponent<CTransform>().angle += 50;
        }
    }
}

void ScenePlatform::sPhysics()
{
    for (auto& one : mEntityManager.getEntities())
    {
        if (!one->hasComponent<CBoundingBox>()) continue;

        for (auto& tile : mEntityManager.getEntities(EntityTag::Tile))
        {            
            if (one->tag() == EntityTag::Tile) continue;

            if (const auto overlap = goldenhand::Physics::boxesOverlap(one->getComponent<CTransform>().pos, one->getComponent<CBoundingBox>().halfSize,
                tile->getComponent<CTransform>().pos, tile->getComponent<CBoundingBox>().halfSize))
            {
                auto& transform = one->getComponent<CTransform>();

                const auto dimensionalOverlap = goldenhand::Physics::boxesDimensionalOverlap(transform.prevPos, one->getComponent<CBoundingBox>().halfSize,
                    tile->getComponent<CTransform>().prevPos, tile->getComponent<CBoundingBox>().halfSize);

                // TODO: refine collision resolution
                if ((transform.prevPos.x <= transform.pos.x) && (dimensionalOverlap.y > 0))
                {
                    transform.pos.x -= overlap->width;
                    transform.velocity.x = 0.f;
                }
                else if ((transform.prevPos.x > transform.pos.x) && (dimensionalOverlap.y > 0))
                {
                    transform.pos.x += overlap->width;
                    transform.velocity.x = 0.f;
                }
                else if ((transform.prevPos.y <= transform.pos.y) && (dimensionalOverlap.x > 0))
                {
                    transform.pos.y -= overlap->height;
                    transform.velocity.y = 0.f;
                }
                else if ((transform.prevPos.y > transform.pos.y) && (dimensionalOverlap.x > 0))
                {
                    transform.pos.y += overlap->height;
                    transform.velocity.y = 0.f;
                }
            }
        }
    }
}

void ScenePlatform::sAnimation()
{
    for (const auto& entity : mEntityManager.getEntities())
    {
        if (entity->tag() == EntityTag::Player) continue;

        auto& animation = entity->getComponent<CAnimation>();
        if (animation.has) mAssetManager.getAnimation(animation.animation).update();
    }

    if (mPlayer->getComponent<CTransform>().velocity.y != 0) mPlayer->getComponent<CAnimation>().animation = Constants::Animation::megaman_jumping;
    else if (mPlayer->getComponent<CTransform>().velocity.x == 0) mPlayer->getComponent<CAnimation>().animation = Constants::Animation::megaman_standing;
    else mPlayer->getComponent<CAnimation>().animation = Constants::Animation::megaman_running;
    
    auto& animation = mAssetManager.getAnimation(mPlayer->getComponent<CAnimation>().animation);

    if (mPlayer->getComponent<CTransform>().angle == 1) animation.getSprite().setScale(1.f, 1.f);
    else animation.getSprite().setScale(-1.f, 1.f);

    animation.update();
}

void ScenePlatform::sLifeSpan()
{
    for (auto& entity : mEntityManager.getEntities())
    {
        if (entity->hasComponent<CLifeSpan>())
        {
            entity->getComponent<CLifeSpan>().remaining--;
            if (entity->getComponent<CLifeSpan>().remaining == 0)
            {
                entity->destroy();
            }
        }
    }

    // respawn player when falls down
    if (mPlayer->getComponent<CTransform>().pos.y > mEngine.windowSize().y)
    {
        mPlayer->destroy();
        spawnPlayer();
        const sf::Vector2f wSize(mEngine.windowSize().x, mEngine.windowSize().y);
        mEngine.setView(sf::View{ {wSize.x / 2, wSize.y / 2}, wSize });
    }
}

void ScenePlatform::spawnPlayer()
{
    mPlayer = mEntityManager.addEntity(EntityTag::Player);

    mPlayer->addComponent<CTransform>(sf::Vector2f{ 200, 400 }, sf::Vector2f{ 0, 0 }, 1.0f);
    mPlayer->addComponent<CAnimation>(Constants::Animation::megaman_standing);
    mPlayer->addComponent<CBoundingBox>(mAssetManager.getAnimation(Constants::Animation::megaman_standing).getSize());
    mPlayer->addComponent<CGravity>();
}

void ScenePlatform::spawnRobot()
{
    auto robot = mEntityManager.addEntity(EntityTag::Robot);

    robot->addComponent<CTransform>(sf::Vector2f{ 500, 400 }, sf::Vector2f{ 0, 0 }, 1.0f);
    robot->addComponent<CAnimation>(Constants::Animation::robot_running);
    robot->addComponent<CBoundingBox>(mAssetManager.getAnimation(Constants::Animation::robot_running).getSize());
    robot->addComponent<CGravity>();
}

void ScenePlatform::shootBlade()
{
    auto blade = mEntityManager.addEntity(EntityTag::Blade);

    const int direction = mPlayer->getComponent<CTransform>().angle;

    blade->addComponent<CTransform>(mPlayer->getComponent<CTransform>().pos, sf::Vector2f{ direction * mBulletConfig.speed, .0f }, .0f);
    blade->addComponent<CAnimation>(Constants::Animation::blade);
    blade->addComponent<CBoundingBox>(mAssetManager.getAnimation(Constants::Animation::blade).getSize());
    blade->addComponent<CLifeSpan>(mBulletConfig.lifespan);
}

void ScenePlatform::saveLevel()
{
    filesystem::path level{ mLevel };
    if (filesystem::exists(level))
    {
        filesystem::remove(level);
    }

    auto fileStream = std::ofstream{ mLevel, ios_base::out };

    for (const auto& decor : mEntityManager.getEntities(EntityTag::Background))
    {
        fileStream << "Background " << decor->getComponent<CAnimation>().animation << std::endl;
    }
    for (const auto& tile : mEntityManager.getEntities(EntityTag::Tile))
    {
        fileStream << "Tile " << tile->getComponent<CAnimation>().animation << " " << tile->getComponent<CTransform>().pos.x << " " << tile->getComponent<CTransform>().pos.y << std::endl;
    }
    const auto playerPos = mPlayer->getComponent<CTransform>().pos;
    fileStream << "Player " << playerPos.x << " " << playerPos.y << " " << mPlayerConfig.runSpeed
        << " " << mPlayerConfig.jumpSpeed << " " << mPlayerConfig.maxSpeed << " " << mPlayerConfig.trapViewRatio;
}

std::shared_ptr<ScenePlatform::Entity> ScenePlatform::findSelectedEntity(const sf::Vector2f spot)
{
    for (const auto entity : mEntityManager.getEntities())
    {
        if (entity->hasComponent<CAnimation>() && entity->hasComponent<CDraggable>())
        {
            const auto size = mAssetManager.getAnimation(entity->getComponent<CAnimation>().animation).getSize();
            const auto position = entity->getComponent<CTransform>().pos;
            if (goldenhand::Physics::isWithinRectangle(sf::Vector2f(spot.x, spot.y), position, sf::Vector2f(size.x, size.y)))
            {
                return entity;
            }
        }
    }

    return nullptr;
}

