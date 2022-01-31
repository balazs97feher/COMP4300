#pragma once

namespace goldenhand
{
    enum class InputEventType
    {
        Default,
        Pressed,
        Released
    };


    // todo: template Action on ActionType
    enum class ActionType
    {
        Default,
        Shoot,
        MoveUp,
        MoveLeft,
        MoveDown,
        MoveRight,
        Quit,
        Select,
        ToggleBBDraw,
        Clone,
        Save
    };

    class Action
    {
    public:
        Action() : type{ ActionType::Default }, eventType{ InputEventType::Default } {}
        Action(const ActionType a, const InputEventType t) : type{ a }, eventType{ t } {}

        ActionType getType() const { return type; }
        InputEventType getEventType() const { return eventType; }

    private:
        ActionType type;
        InputEventType eventType;
    };
} // namespace
