#pragma once
#include "BlackboardBase.h"
#include "Nodes.h"

struct IsPlayerInRangeParameters : ParamsForCondition
{
    float Range = 100.0f;
    HBlackboardKeyValue DistanceToPlayerKey;
    void DrawImGui(HBlackboard* blackboard) override
    {
        DrawFloatValue("Range", Range);
        DrawBlackboardFloatKeySelector("Distance To Player", DistanceToPlayerKey, blackboard);
    }
};
class IsPlayerInRangeCondition : public HCondition
{
public:
    IsPlayerInRangeCondition(const std::string& name, const IsPlayerInRangeParameters& params = IsPlayerInRangeParameters{})
        : HCondition(name, params), m_Range(params.Range), m_DistanceToPlayer(params.DistanceToPlayerKey) {}

    void OnStart() override;
    bool CheckCondition() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    float m_Range;
    HBlackboardKeyValue m_DistanceToPlayer;
};