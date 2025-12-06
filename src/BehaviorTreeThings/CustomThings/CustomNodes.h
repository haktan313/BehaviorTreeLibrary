#pragma once
#include <iostream>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "../Core/Nodes.h"
#include "../Editor/NodeEditorStructsAndEnums.h"

struct ActionNodeParams : Params
{
    ActionNodeParams(float speed = 1.0f) : Speed(speed) {}
    float Speed = 1.0f;
    void DrawImGui() override
    {
        ImGui::InputFloat("Speed", &Speed);
    }
};
class ActionNode : public HActionNode
{
public:
    ActionNode(const std::string& name, const ActionNodeParams& params) : HActionNode(name), m_Speed(params.Speed) {}
    
    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
protected:
    float m_TickCount = 0;
    float m_Speed;
};

struct MoveToNodeParams : Params
{
    float AcceptanceRadius = 5.0f;
    void DrawImGui() override
    {
        ImGui::InputFloat("Acceptance Radius", &AcceptanceRadius);
    }
};
class MoveToNode : public HActionNode
{
public:
    MoveToNode(const std::string& name, const MoveToNodeParams& params) : HActionNode(name), m_AcceptanceRadius(params.AcceptanceRadius) {}

    void OnStart() override { std::cout << "MoveTo Node Started: " << m_Name << " with Acceptance Radius: " << m_AcceptanceRadius << std::endl; HActionNode::OnStart(); }
    NodeStatus Update() override { return NodeStatus::SUCCESS; }
    void OnFinished() override { HActionNode::OnFinished(); }
    void OnAbort() override { HActionNode::OnAbort(); }
private:
    float m_AcceptanceRadius;
};


struct AlwaysTrueConditionParams : ParamsForCondition
{
    bool bInverse = false;
    void DrawImGui() override
    {
        ImGui::Checkbox("Inverse", &bInverse);
    }
};
class AlwaysTrueCondition : public HCondition
{
public:
    AlwaysTrueCondition(const std::string& name, const AlwaysTrueConditionParams& params) : HCondition(name), m_bInverse(params.bInverse) {}

    void OnStart() override { HCondition::OnStart(); }
    NodeStatus Update() override { return NodeStatus::SUCCESS; }
    void OnFinished() override { HCondition::OnFinished(); }
    void OnAbort() override { HCondition::OnAbort(); }
private:
    bool m_bInverse = false;
};

class AlwaysFalseCondition : public HCondition
{
public:
    AlwaysFalseCondition(const std::string& name) : HCondition(name) {}

    void OnStart() override { HCondition::OnStart(); }
    NodeStatus Update() override { return NodeStatus::FAILURE; }
    void OnFinished() override { HCondition::OnFinished(); }
    void OnAbort() override { HCondition::OnAbort(); }
};

struct CanSeePlayerConditionParams : ParamsForCondition
{
    bool bInverse = false;
    void DrawImGui() override
    {
        ImGui::Checkbox("Inverse", &bInverse);
    }
};
class CanSeePlayerCondition : public HCondition
{
public:
    CanSeePlayerCondition(const std::string& name, const CanSeePlayerConditionParams& params) : HCondition(name), m_bInverse(params.bInverse) {}

    void OnStart() override { HCondition::OnStart(); }
    NodeStatus Update() override;
    void OnFinished() override { HCondition::OnFinished(); }
    void OnAbort() override { HCondition::OnAbort(); }
private:
    bool m_bInverse = false;
};


struct InverterDecoratorParams : Params
{
    bool dummy = false;
    void DrawImGui() override
    {
        ImGui::Checkbox("Dummy", &dummy);
    }
};
class InverterDecorator : public HDecorator
{
public:
    InverterDecorator(const std::string& name, const InverterDecoratorParams& params) : HDecorator(name) {}

    void OnStart() override { HDecorator::OnStart(); }
    NodeStatus Update() override;
    void OnFinished() override { HDecorator::OnFinished(); }
    void OnAbort() override { HDecorator::OnAbort(); }
};


struct SucceederDecoratorParams : Params
{
    int dummy = 0;
    void DrawImGui() override
    {
        ImGui::InputInt("Dummy", &dummy);
    }
};
class SucceederDecorator : public HDecorator
{
public:
    SucceederDecorator(const std::string& name, const SucceederDecoratorParams& params) : HDecorator(name) {}

    void OnStart() override { HDecorator::OnStart(); }
    NodeStatus Update() override { return NodeStatus::SUCCESS; }
    void OnFinished() override { HDecorator::OnFinished(); }
    void OnAbort() override { HDecorator::OnAbort(); }
};