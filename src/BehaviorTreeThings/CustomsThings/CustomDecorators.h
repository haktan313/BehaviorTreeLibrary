#pragma once
#include "imgui.h"
#include "Nodes.h"

struct CooldownDecoratorParameters : ParamsForDecorator
{
    float CooldownTime = 5.0f;
    void DrawImGui() override
    {
        ImGui::InputFloat("Cooldown Time", &CooldownTime);
    }
};
class CooldownDecorator : public HDecorator
{
public:
    CooldownDecorator(const std::string& name, const CooldownDecoratorParameters& params = CooldownDecoratorParameters{})
        : HDecorator(name, params), m_CooldownTime(params.CooldownTime), m_LastExecutionTime(-FLT_MAX) {}
    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    float m_CooldownTime;
    float m_LastExecutionTime;
};

struct TimeLimitDecoratorParameters : ParamsForDecorator
{
    float TimeLimit = 10.0f;
    void DrawImGui() override
    {
        ImGui::InputFloat("Time Limit", &TimeLimit);
    }
};
class TimeLimitDecorator : public HDecorator
{
public:
    TimeLimitDecorator(const std::string& name, const TimeLimitDecoratorParameters& params = TimeLimitDecoratorParameters{})
        : HDecorator(name, params), m_TimeLimit(params.TimeLimit), m_StartTime(0.0f) {}
    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    float m_TimeLimit;
    float m_StartTime;
};
