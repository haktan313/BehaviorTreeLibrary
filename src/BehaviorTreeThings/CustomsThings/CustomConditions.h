#pragma once
#include "imgui.h"
#include "Nodes.h"

struct IsPlayerInRangeParameters : ParamsForCondition
{
    float Range = 100.0f;
    void DrawImGui() override
    {
        ImGui::InputFloat("Range", &Range);
    }
};
class IsPlayerInRangeCondition : public HCondition
{
public:
    IsPlayerInRangeCondition(const std::string& name, const IsPlayerInRangeParameters& params = IsPlayerInRangeParameters{})
        : HCondition(name, params), m_Range(params.Range) {}

    void OnStart() override;
    bool CheckCondition() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    float m_Range;
};