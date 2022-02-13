#pragma once

#include "Action.h"

#include <unordered_map>

namespace goldenhand
{
    class GameEngine;
    
    typedef std::unordered_map<int, ActionType> ActionMap;

    class Scene
    {
    public:
        Scene(GameEngine& engine);

        virtual void initialize() = 0;

        virtual void update() = 0;
        virtual void sDoAction(const Action action) = 0;
        virtual void sRender() = 0;

        const ActionMap& getKbdActionMap() const;
        const ActionMap& getMouseActionMap() const;
        void registerKbdAction(const int code, const ActionType action);
        void registerMouseAction(const int code, const ActionType action);

    protected:
        GameEngine& mEngine;
        int mCurrentFrame;
        bool mPaused;
        bool mHasEnded;
        ActionMap mKbdActionMap;
        ActionMap mMouseActionMap;
    };
} // namespace goldenhand
