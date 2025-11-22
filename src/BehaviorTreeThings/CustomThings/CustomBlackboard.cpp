#include "CustomBlackboard.h"

EnemyBlackboard::EnemyBlackboard()
{
    CreateBoolValue("IsPlayerVisible", false);
    CreateFloatValue("Health", 100.0f);
    CreateIntValue("AmmoCount", 30);
    CreateStringValue("CurrentState", "Idle");
}