#pragma once
#include "imgui.h"
#include "Nodes.h"

struct ChangeResultOfTheNodeParameters : ParamsForDecorator
{
    NodeStatus NewResult = NodeStatus::SUCCESS;
    void DrawImGui() override
    {
        const char* items[] = { "SUCCESS", "FAILURE", "RUNNING" };
        int currentItem = static_cast<int>(NewResult);
        if (ImGui::Combo("New Result", &currentItem, items, IM_ARRAYSIZE(items)))
        {
            NewResult = static_cast<NodeStatus>(currentItem);
        }
    }
};
class ChangeResultOfTheNodeDecorator : public HDecorator
{
public:
    ChangeResultOfTheNodeDecorator(const std::string& name, const ChangeResultOfTheNodeParameters& params = ChangeResultOfTheNodeParameters{})
        : HDecorator(name, params), m_NewResult(params.NewResult) {}
    void OnStart() override;
    bool CanExecute() override;
    void OnFinishedResult(NodeStatus& status) override;
    void OnFinished() override;
    void OnAbort() override;
private:
    NodeStatus m_NewResult;
};

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
        : HDecorator(name, params), m_CooldownTime(params.CooldownTime), m_LastExecutionTime(-params.CooldownTime) {}
    void OnStart() override;
    bool CanExecute() override;
    void OnFinishedResult(NodeStatus& status) override;
    void OnFinished() override;
    void OnAbort() override;
private:
    float m_CooldownTime;
    float m_LastExecutionTime;
};