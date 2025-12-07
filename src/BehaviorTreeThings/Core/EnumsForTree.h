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
