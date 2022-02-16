#include "ConfigReader.h"
#include "Constants.h"
#include "GameEngine.h"
#include "ScenePlatform.h"
#include "Utils.h"

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;
using goldenhand::Animation;

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
    //registerKbdAction(sf::Keyboard::L, ActionType::Save);

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
            configuration >> mPlayerConfig.runSpeed >> mPlayerConfig.jumpSpeed >> mPlayerConfig.maxSpeed >> mPlayerConfig.trapViewRatio;
        }
        else if (configType == "Robot")
        {
            RobotConfig robotConfig;
            configuration >> robotConfig.startPosX >> robotConfig.startPosY >> robotConfig.cooldown;
            spawnRobot({ robotConfig.startPosX, robotConfig.startPosY }, robotConfig.cooldown);
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
            background->addComponent<Animation>(mAssetManager.getAnimation(name));
            auto& animation = mAssetManager.getAnimation(name);
            animation.getSprite().setScale(static_cast<float>(mEngine.windowSize().x) / animation.getSize().x,
                static_cast<float>(mEngine.windowSize().y) / animation.getSize().y);
        }
        else if (configType == "Tile")
        {
            configuration >> name >> x >> y;
            auto tile = mEntityManager.addEntity(EntityTag::Tile);
            tile->addComponent<CTransform>(sf::Vector2f{ x, y }, sf::Vector2f{ 0, 0 }, 0.f);
            tile->addComponent<Animation>(mAssetManager.getAnimation(name));
            tile->addComponent<CBoundingBox>(mAssetManager.getAnimation(name).getSize());
            tile->addComponent<CDraggable>();
        }

        configuration >> configType;
    }

    spawnPlayer();
}

void ScenePlatform::update()
{
    if (!mHasEnded)
    {
        mEntityManager.update();

        sLifeSpan();
        sView();
        sMovement();
        sRobotAttack();
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
    //case ActionType::Save:
    //    if (action.getEventType() == InputEventType::Released) saveLevel();
    //    break;
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
            shootBlade(mPlayer, mEngine.mousePos() - mPlayer->getComponent<CTransform>().pos);
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
        if (mTextureDraw && entity->hasComponent<Animation>())
        {
            auto& sprite = entity->getComponent<Animation>().getSprite();
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
                entity->getComponent<CTransform>().angle = 0;
            }
            else if (entity->getComponent<CTransform>().pos.x < entity->getComponent<CTransform>().prevPos.x)
            {
                entity->getComponent<CTransform>().angle = pi;
            }
        }
        else
        {
            entity->getComponent<CTransform>().angle += mBulletConfig.rotation;
        }
    }
}

void ScenePlatform::sPhysics()
{
    for (auto& one : mEntityManager.getEntities())
    {
        if (!one->hasComponent<CBoundingBox>()) continue;

        // check collision with tiles
        if (one->tag() != EntityTag::Tile)
        {
            for (auto& tile : mEntityManager.getEntities(EntityTag::Tile))
            {
                if (const auto overlap = goldenhand::Physics::boxesOverlap(one->getComponent<CTransform>().pos, one->getComponent<CBoundingBox>().halfSize,
                    tile->getComponent<CTransform>().pos, tile->getComponent<CBoundingBox>().halfSize))
                {
                    if (one->tag() == EntityTag::Blade)
                    {
                        one->destroy();
                        continue;
                    }

                    auto& transform = one->getComponent<CTransform>();

                    const auto dimensionalOverlap = goldenhand::Physics::boxesDimensionalOverlap(transform.prevPos, one->getComponent<CBoundingBox>().halfSize,
                        tile->getComponent<CTransform>().prevPos, tile->getComponent<CBoundingBox>().halfSize);

                    // TODO: refine collision resolution
                    if ((transform.prevPos.x <= transform.pos.x) && (dimensionalOverlap.y > 0))
                    {
                        transform.pos.x -= overlap->width;
                        if (one->tag() == EntityTag::Robot)
                        {
                            transform.velocity.x *= -1;
                        }
                        else
                        {
                            transform.velocity.x = 0.f;
                        }
                    }
                    else if ((transform.prevPos.x > transform.pos.x) && (dimensionalOverlap.y > 0))
                    {
                        transform.pos.x += overlap->width;
                        if (one->tag() == EntityTag::Robot)
                        {
                            transform.velocity.x *= -1;
                        }
                        else
                        {
                            transform.velocity.x = 0.f;
                        }
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

        // check collision between blades and entities
        for (auto& blade : mEntityManager.getEntities(EntityTag::Blade))
        {
            if (one->tag() == EntityTag::Blade || one->tag() == EntityTag::Tile) continue;
            if ((one->tag() == EntityTag::Robot || one->tag() == EntityTag::Player) && mBladeOrigin[blade->id()] == one->id()) continue;

            if ((one->tag() == EntityTag::Robot || one->tag() == EntityTag::Player) && goldenhand::Physics::boxesOverlap(one->getComponent<CTransform>().pos,
                one->getComponent<CBoundingBox>().halfSize, blade->getComponent<CTransform>().pos, blade->getComponent<CBoundingBox>().halfSize))
            {
                blade->destroy();
                mBladeOrigin.erase(blade->id());

                if (one->tag() == EntityTag::Robot && mBladeOrigin[blade->id()] == mPlayer->id())
                {
                    destroyRobot(one);
                }
                else if (one->tag() == EntityTag::Player)
                {
                    mPlayer->destroy();
                    spawnPlayer();
                }
            }
        }
    }

    if (mPlayer->getComponent<CTransform>().velocity.y != 0)
    {
        mPlayer->getComponent<CState>().changeState(CharacterState::Jumping, mCurrentFrame);
    }
    else if (mPlayer->getComponent<CTransform>().velocity.x == 0)
    {
        mPlayer->getComponent<CState>().changeState(CharacterState::Standing, mCurrentFrame);
    }
    else
    {
        mPlayer->getComponent<CState>().changeState(CharacterState::Running, mCurrentFrame);
    }
}

void ScenePlatform::sAnimation()
{
    for (const auto& entity : mEntityManager.getEntities())
    {
        if (entity->hasComponent<Animation>())
        {
            if (entity->getComponent<CState>().stateChangedInFrame(mCurrentFrame))
            {
                if (entity->tag() == EntityTag::Robot)
                {
                    auto& animation = entity->getComponent<Animation>();
                    switch (entity->getComponent<CState>().current())
                    {
                        case CharacterState::Running:
                            animation = mAssetManager.getAnimation(Constants::Animation::robot_running);
                            break;
                        case CharacterState::Shooting:
                            animation = mAssetManager.getAnimation(Constants::Animation::robot_shooting);
                            break;
                        case CharacterState::Dying:
                            animation = mAssetManager.getAnimation(Constants::Animation::robot_dying);
                            animation.setLoop(false);
                            break;
                    }
                }
                else if (entity->tag() == EntityTag::Player)
                {
                    auto& animation = entity->getComponent<Animation>();
                    switch (entity->getComponent<CState>().current())
                    {
                        case CharacterState::Standing:
                            animation = mAssetManager.getAnimation(Constants::Animation::megaman_standing);
                            break;
                        case CharacterState::Running:
                            animation = mAssetManager.getAnimation(Constants::Animation::megaman_running);
                            break;
                        case CharacterState::Jumping:
                            animation = mAssetManager.getAnimation(Constants::Animation::megaman_jumping);
                            break;
                    }
                }
            }

            auto& animation = entity->getComponent<Animation>();
            if (animation.hasEnded())
            {
                entity->destroy();
            }
            else
            {
                if (goldenhand::equal(entity->getComponent<CTransform>().angle, 0.f)) animation.getSprite().setScale(1.f, 1.f);
                else if (goldenhand::equal(entity->getComponent<CTransform>().angle, pi)) animation.getSprite().setScale(-1.f, 1.f);
            
                animation.update();
            }
        }
    }
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
                if (entity->tag() == EntityTag::Blade) mBladeOrigin.erase(entity->id());
            }
        }
    }

    // respawn player when falls down
    if (mPlayer->getComponent<CTransform>().pos.y > mEngine.windowSize().y)
    {
        mPlayer->destroy();
        spawnPlayer();
    }
}

void ScenePlatform::sRobotAttack()
{
    for (auto robot : mEntityManager.getEntities(EntityTag::Robot))
    {
        if (robot->getComponent<CState>().current() == CharacterState::Dying) continue;

        if (auto& rem = robot->getComponent<CCooldown>().remaining; rem > 0)
        {
            rem--;
            continue;
        }
        
        if (const auto dir = playerWithinSight(robot))
        {
            shootBlade(robot, dir.value());
            robot->getComponent<CCooldown>().remaining = robot->getComponent<CCooldown>().total;
        }
    }
}

void ScenePlatform::spawnPlayer()
{
    mPlayer = mEntityManager.addEntity(EntityTag::Player);

    mPlayer->addComponent<CTransform>(sf::Vector2f{ 200, 400 }, sf::Vector2f{ 0, 0 }, 1.0f);
    mPlayer->addComponent<Animation>(mAssetManager.getAnimation(Constants::Animation::megaman_standing));
    mPlayer->addComponent<CBoundingBox>(mAssetManager.getAnimation(Constants::Animation::megaman_standing).getSize());
    mPlayer->addComponent<CGravity>();
    mPlayer->addComponent<CState>(CharacterState::Running);

    const sf::Vector2f wSize(mEngine.windowSize().x, mEngine.windowSize().y);
    mEngine.setView(sf::View{ {wSize.x / 2, wSize.y / 2}, wSize });
}

void ScenePlatform::spawnRobot(const sf::Vector2f startPos, const int cooldown)
{
    auto robot = mEntityManager.addEntity(EntityTag::Robot);

    robot->addComponent<CTransform>(startPos, sf::Vector2f{ 2, 0 }, 0);
    robot->addComponent<Animation>(mAssetManager.getAnimation(Constants::Animation::robot_running));
    robot->addComponent<CBoundingBox>(mAssetManager.getAnimation(Constants::Animation::robot_running).getSize());
    robot->addComponent<CGravity>();
    robot->addComponent<CCooldown>(cooldown);
    robot->addComponent<CState>(CharacterState::Running);
}

void ScenePlatform::destroyRobot(std::shared_ptr<Entity> robot)
{
    robot->getComponent<CTransform>().velocity = {};
    robot->getComponent<CState>().changeState(CharacterState::Dying, mCurrentFrame);
}

std::optional<sf::Vector2f> ScenePlatform::playerWithinSight(std::shared_ptr<Entity> robot)
{
    // check if the robot faces toward the player
    const auto angle = goldenhand::angle(goldenhand::dirVector(robot->getComponent<CTransform>().angle),
        mPlayer->getComponent<CTransform>().pos - robot->getComponent<CTransform>().pos);
    if (angle > pi / 2 || angle < -pi / 2)
    {
        robot->getComponent<CState>().changeState(CharacterState::Running, mCurrentFrame);
        return nullopt;
    }

    for (const auto entity : mEntityManager.getEntities())
    {
        if (entity->tag() != EntityTag::Player && entity->tag() != EntityTag::Background && entity->tag() != EntityTag::Robot &&
            entity->tag() != EntityTag::Blade)
        {
            const auto entitySize = entity->getComponent<Animation>().getSize();

            if (goldenhand::Physics::lineSegmentRectangleIntersect(mPlayer->getComponent<CTransform>().pos, robot->getComponent<CTransform>().pos,
                entity->getComponent<CTransform>().pos, { entitySize.x / 2.f, entitySize.y / 2.f}))
            {
                robot->getComponent<CState>().changeState(CharacterState::Running, mCurrentFrame);
                return std::nullopt;
            }
        }
    }

    robot->getComponent<CState>().changeState(CharacterState::Shooting, mCurrentFrame);
    
    return mPlayer->getComponent<CTransform>().pos - robot->getComponent<CTransform>().pos;
}

void ScenePlatform::shootBlade(std::shared_ptr<Entity> shooter, const sf::Vector2f& dir)
{
    const float angle = atan2(dir.y, dir.x);

    auto blade = mEntityManager.addEntity(EntityTag::Blade);
    blade->addComponent<CTransform>(shooter->getComponent<CTransform>().pos, sf::Vector2f{ cos(angle), sin(angle) } * mBulletConfig.speed, .0f);
    blade->addComponent<Animation>(mAssetManager.getAnimation(Constants::Animation::blade));
    blade->addComponent<CBoundingBox>(mAssetManager.getAnimation(Constants::Animation::blade).getSize());
    blade->addComponent<CLifeSpan>(mBulletConfig.lifespan);

    mBladeOrigin[blade->id()] = shooter->id();
}

//void ScenePlatform::saveLevel()
//{
//    filesystem::path level{ mLevel };
//    if (filesystem::exists(level))
//    {
//        filesystem::remove(level);
//    }
//
//    auto fileStream = std::ofstream{ mLevel, ios_base::out };
//
//    for (const auto& decor : mEntityManager.getEntities(EntityTag::Background))
//    {
//        fileStream << "Background " << decor->getComponent<CAnimation>().animation << std::endl;
//    }
//    for (const auto& tile : mEntityManager.getEntities(EntityTag::Tile))
//    {
//        fileStream << "Tile " << tile->getComponent<CAnimation>().animation << " " << tile->getComponent<CTransform>().pos.x << " " << tile->getComponent<CTransform>().pos.y << std::endl;
//    }
//    const auto playerPos = mPlayer->getComponent<CTransform>().pos;
//    fileStream << "Player " << " " << mPlayerConfig.runSpeed
//        << " " << mPlayerConfig.jumpSpeed << " " << mPlayerConfig.maxSpeed << " " << mPlayerConfig.trapViewRatio << std::endl;
//    fileStream << "Bullet " << mBulletConfig.speed << " " << mBulletConfig.rotation << " " << mBulletConfig.lifespan;
//}

std::shared_ptr<ScenePlatform::Entity> ScenePlatform::findSelectedEntity(const sf::Vector2f spot)
{
    for (const auto entity : mEntityManager.getEntities())
    {
        if (entity->hasComponent<Animation>() && entity->hasComponent<CDraggable>())
        {
            const auto size = entity->getComponent<Animation>().getSize();
            const auto position = entity->getComponent<CTransform>().pos;
            if (goldenhand::Physics::isWithinRectangle(sf::Vector2f(spot.x, spot.y), position, sf::Vector2f(size.x, size.y)))
            {
                return entity;
            }
        }
    }

    return nullptr;
}

