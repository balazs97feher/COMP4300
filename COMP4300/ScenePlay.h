#pragma once

#include "Scene.h"

class ScenePlay : public Scene
{
public:
    ScenePlay(GameEngine& engine);

    virtual void initialize() override;

    virtual void update() override;
    virtual void sDoAction(const Action action) override;
    virtual void sRender() override;

private:
    void sMovement();
    void sLifespan();
    void sAttack();
    void sEnemySpawner();
    void sCollision();

};

