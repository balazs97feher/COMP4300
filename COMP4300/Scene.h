#pragma once

#include "Action.h"
#include "EntityManager.h"

#include <unordered_map>

class GameEngine;

typedef std::unordered_map<int, Action> ActionMap;

class Scene
{
public:
    Scene(GameEngine& engine);

    virtual void update() = 0;
    virtual void sDoAction(const Action action) = 0;
    virtual void sRender() = 0;

    const ActionMap& getActionMap() const;
    void registerAction(const int code, const Action action);

private:
    GameEngine& mEngine;
    EntityManager mEntityManager;
    int mCurrentFrame;
    bool mPaused;
    bool mHasEnded;
    ActionMap mActionMap;

};

