#pragma once

#include "Action.h"

#include <unordered_map>

namespace goldenhand
{
    class GameEngine;
    
    typedef std::unordered_map<int, ActionType> ActionMap;
    
    // todo: unique id for scenes without having to change Scene base class
    enum class SceneId
    {
        Menu,
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
        int mCurrentFrame;
        bool mPaused;
        bool mHasEnded;
        ActionMap mKbdActionMap;
    };
} // namespace goldenhand
