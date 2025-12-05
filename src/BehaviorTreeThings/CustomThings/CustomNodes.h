#pragma once
#include <iostream>

#include "../Core/Nodes.h"

struct ActionNodeParams
{
    float Speed = 1.0f;
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

struct MoveToNodeParams
{
    float AcceptanceRadius = 5.0f;
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

class AlwaysTrueCondition : public HCondition
{
public:
    AlwaysTrueCondition(const std::string& name) : HCondition(name) {}

    void OnStart() override { HCondition::OnStart(); }
    NodeStatus Update() override { return NodeStatus::SUCCESS; }
    void OnFinished() override { HCondition::OnFinished(); }
    void OnAbort() override { HCondition::OnAbort(); }
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

class CanSeePlayerCondition : public HCondition
{
public:
    CanSeePlayerCondition(const std::string& name, bool bInverse) : HCondition(name), m_bInverse(bInverse) {}

    void OnStart() override { HCondition::OnStart(); }
    NodeStatus Update() override;
    void OnFinished() override { HCondition::OnFinished(); }
    void OnAbort() override { HCondition::OnAbort(); }
private:
    bool m_bInverse = false;
};

class InverterDecorator : public HDecorator
{
public:
    InverterDecorator(const std::string& name) : HDecorator(name) {}

    void OnStart() override { HDecorator::OnStart(); }
    NodeStatus Update() override;
    void OnFinished() override { HDecorator::OnFinished(); }
    void OnAbort() override { HDecorator::OnAbort(); }
};