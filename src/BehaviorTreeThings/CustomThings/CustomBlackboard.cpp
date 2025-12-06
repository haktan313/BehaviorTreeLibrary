#include "CustomBlackboard.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

EnemyBlackboard::EnemyBlackboard()
{
    CreateBoolValue("IsPlayerVisible", true);
    CreateFloatValue("Health", 100.0f);
    CreateIntValue("AmmoCount", 30);
    CreateStringValue("CurrentState", "Idle");
}

EnemySoilderBlackboard::EnemySoilderBlackboard()
{
    CreateBoolValue("CanAttack", true);
    CreateFloatValue("RateAmount", 30.0f);
}

