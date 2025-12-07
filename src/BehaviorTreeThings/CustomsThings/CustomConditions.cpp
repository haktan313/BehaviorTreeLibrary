#include "CustomConditions.h"

#include <iostream>

#include "BlackboardBase.h"


void IsPlayerInRangeCondition::OnStart()
{
    HCondition::OnStart();
    std::cout << "IsPlayerInRangeCondition started.\n";
}

bool IsPlayerInRangeCondition::CheckCondition()
{
    if (GetBlackboard().GetFloatValue("DistanceToPlayer") <= m_Range)
    {
        std::cout << "Player is within range (" << m_Range << ").\n";
        GetBlackboard().SetBoolValue("IsPlayerInRange", true);
        return true;
    }
    std::cout << "Player is out of range (" << m_Range << ").\n";
    GetBlackboard().SetBoolValue("IsPlayerInRange", false);
    return false;
}

void IsPlayerInRangeCondition::OnFinished()
{
    HCondition::OnFinished();
    std::cout << "IsPlayerInRangeCondition finished.\n";
}

void IsPlayerInRangeCondition::OnAbort()
{
    HCondition::OnAbort();
    std::cout << "IsPlayerInRangeCondition aborted.\n";
}
