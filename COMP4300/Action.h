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
    MoveRight
};

class Action
{
public:
    Action(const ActionType a, const InputEventType t) : type{ a }, eventType{ t } {}

    const ActionType type;
    const InputEventType eventType;
};

