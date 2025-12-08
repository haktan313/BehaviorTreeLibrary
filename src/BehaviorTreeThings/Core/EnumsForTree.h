#pragma once

enum class NodeStatus
{
    SUCCESS,
    FAILURE,
    RUNNING
};

enum class PriorityType
{
    None,
    Self,
    LowerPriority,
    Both
};

enum class HNodeType
{
    None,
    Action,
    Condition,
    Decorator,
    Composite,
    Root
};

typedef std::string HBlackboardKeyValue;