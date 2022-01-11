#include "Scene.h"

Scene::Scene(GameEngine& engine) : mEngine{engine}, mCurrentFrame{0}, mPaused{false}, mHasEnded{false}
{
}

const ActionMap& Scene::getKbdActionMap() const
{
    return mKbdActionMap;
}

void Scene::registerAction(const int code, const ActionType action)
{
    mKbdActionMap[code] = action;
}
