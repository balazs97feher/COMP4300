#include "Scene.h"

goldenhand::Scene::Scene(GameEngine& engine) : mEngine{engine}, mCurrentFrame{0}, mPaused{false}, mHasEnded{false}
{
}

const goldenhand::ActionMap& goldenhand::Scene::getKbdActionMap() const
{
    return mKbdActionMap;
}

void goldenhand::Scene::registerAction(const int code, const ActionType action)
{
    mKbdActionMap[code] = action;
}
