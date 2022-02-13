#include "Scene.h"

goldenhand::Scene::Scene(GameEngine& engine) : mEngine{engine}, mCurrentFrame{0}, mPaused{false}, mHasEnded{false}
{
}

const goldenhand::ActionMap& goldenhand::Scene::getKbdActionMap() const
{
    return mKbdActionMap;
}

const goldenhand::ActionMap& goldenhand::Scene::getMouseActionMap() const
{
    return mMouseActionMap;
}

void goldenhand::Scene::registerKbdAction(const int code, const ActionType action)
{
    mKbdActionMap[code] = action;
}

void goldenhand::Scene::registerMouseAction(const int code, const ActionType action)
{
    mMouseActionMap[code] = action;
}
