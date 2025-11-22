#pragma once
#include "../Core/Nodes.h"

class ActionNode : public HActionNode
{
public:
    ActionNode(const std::string& name, float speed) : HActionNode(name), m_Speed(speed) {}
    
    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
protected:
    float m_TickCount = 0;
    float m_Speed;
};

class AlwaysTrueCondition : public HCondition
{
public:
    AlwaysTrueCondition(const std::string& name) : HCondition(name) {}

    void OnStart() override {}
    NodeStatus Update() override { return NodeStatus::SUCCESS; }
    void OnFinished() override {}
    void OnAbort() override {}
};

class AlwaysFalseCondition : public HCondition
{
public:
    AlwaysFalseCondition(const std::string& name) : HCondition(name) {}

    void OnStart() override {}
    NodeStatus Update() override { return NodeStatus::FAILURE; }
    void OnFinished() override {}
    void OnAbort() override {}
};

class CanSeePlayerCondition : public HCondition
{
public:
    CanSeePlayerCondition(const std::string& name, bool bInverse) : HCondition(name), m_bInverse(bInverse) {}

    void OnStart() override {}
    NodeStatus Update() override;
    void OnFinished() override {}
    void OnAbort() override {}
private:
    bool m_bInverse = false;
};

class InverterDecorator : public HDecorator
{
public:
    InverterDecorator(const std::string& name) : HDecorator(name) {}

    void OnStart() override {}
    NodeStatus Update() override;
    void OnFinished() override {}
    void OnAbort() override {}
};