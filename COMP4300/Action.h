#pragma once

enum class InputEventType
{
    Default,
    Pressed,
    Released
};

enum class ActionType
{
    Default,
    Shoot,
    MoveUp,
    MoveLeft,
    MoveDown,
    MoveRight,
    Quit,
    Select
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

