#include "Scene.h"

Scene::Scene(GameEngine& engine) : mEngine{engine}, mCurrentFrame{0}, mPaused{false}, mHasEnded{false}
{
}

const ActionMap& Scene::getActionMap() const
{
    return mActionMap;
}

void Scene::registerAction(const int code, const ActionType action)
{
    mActionMap[code] = action;
}
