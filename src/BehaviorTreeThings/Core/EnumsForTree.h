#pragma once

enum class NodeStatus
{
    SUCCESS,
    FAILURE,
    RUNNING
};

enum class PriortyType
{
    None,
    Self,
    LowerPriority,
    Both
};
