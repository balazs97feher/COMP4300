#include "SceneAnimation.h"

SceneAnimation::SceneAnimation(GameEngine& engine) : Scene{engine}, mAssetManager{ "./config/" }
{
    initialize();
}

void SceneAnimation::initialize()
{
    mAssetManager.loadAssets();
}

void SceneAnimation::update()
{
}

void SceneAnimation::sDoAction(const Action action)
{
}

void SceneAnimation::sRender()
{
}
