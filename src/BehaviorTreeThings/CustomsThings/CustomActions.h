#pragma once
#include "imgui.h"
#include "Nodes.h"


struct MoveToParameters : Params
{
    bool bToPlayer = true;
    float StopDistance = 10.0f;
    float MoveSpeed = 5.0f;
    void DrawImGui(HBlackboard* blackboard) override
    {
        DrawBoolValue("Move To Player", bToPlayer);
        DrawFloatValue("Stop Distance", StopDistance);
        DrawFloatValue("Move Speed", MoveSpeed);
    }
};
class MoveToAction : public HActionNode
{
public:
    MoveToAction(const std::string& name, const MoveToParameters& params = MoveToParameters{})
        : HActionNode(name, params), m_bToPlayer(params.bToPlayer), m_StopDistance(params.StopDistance), m_MoveSpeed(params.MoveSpeed / 10.f) {}
    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    bool m_bToPlayer;
    float m_StopDistance;
    float m_MoveSpeed;
    float m_DistanceToTarget;
};

struct MeleeEnemyAttackActionParameters : Params
{
    HBlackboardKeyValue AttackPowerKey;
    float AttackDuration = 10.0f;
    void DrawImGui(HBlackboard* blackboard) override
    {
        DrawBlackboardIntKeySelector("Attack Power", AttackPowerKey, blackboard);
        DrawFloatValue("Attack Duration", AttackDuration);
    }
};
class MeleeEnemyAttackAction : public HActionNode
{
public:
    MeleeEnemyAttackAction(const std::string& name, const MeleeEnemyAttackActionParameters& params = MeleeEnemyAttackActionParameters{})
        : HActionNode(name, params), m_AttackPowerKey(params.AttackPowerKey), m_AttackDuration(params.AttackDuration) {}

    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    HBlackboardKeyValue m_AttackPowerKey;
    float m_AttackDuration;
    float m_ElapsedTime = 0.0f;
};