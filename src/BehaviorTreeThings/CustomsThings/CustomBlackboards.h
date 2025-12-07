#pragma once
#include "BlackboardBase.h"

class MeleeEnemyBlackboard : public HBlackboard
{
public:
    MeleeEnemyBlackboard()
    {
        CreateBoolValue("IsPlayerInRange", false);
        CreateBoolValue("IsPlayerAttacking", false);
        CreateBoolValue("CanAttack", true);
        
        CreateFloatValue("DistanceToPlayer", 200.0f);
        CreateFloatValue("Health", 100.0f);
        
        CreateIntValue("AttackPower", 10);
        
        CreateStringValue("CurrentState", "Idle");
    }
};

class RangedEnemyBlackboard : public HBlackboard
{
public:
    RangedEnemyBlackboard()
    {
        CreateBoolValue("IsPlayerInSight", false);
        CreateBoolValue("ShouldReload", false);
        CreateBoolValue("CanShoot", true);

        CreateFloatValue("DistanceToPlayer", 300.0f);
        CreateFloatValue("Health", 100.0f);
        
        CreateIntValue("AmmoCount", 5);
        CreateIntValue("DamageAmount", 15);

        CreateStringValue("CurrentState", "Idle");
        
    }
};