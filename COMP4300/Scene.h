#pragma once

#include "Action.h"
#include "EntityManager.h"

#include <unordered_map>

class GameEngine;

typedef std::unordered_map<int, ActionType> ActionMap;

enum class SceneId
{
    Menu,
    Play,
    Animation
};

class Scene
{
public:
    Scene(GameEngine& engine);

    virtual void initialize() = 0;

    virtual void update() = 0;
    virtual void sDoAction(const Action action) = 0;
    virtual void sRender() = 0;

    const ActionMap& getKbdActionMap() const;
    void registerAction(const int code, const ActionType action);

protected:
    GameEngine& mEngine;
    EntityManager mEntityManager;
    int mCurrentFrame;
    bool mPaused;
    bool mHasEnded;
    ActionMap mKbdActionMap;

};

