#pragma once
#include <memory>
#include <string>
#include <vector>
#include "EnumsForTree.h"

class HCondition;
class HBlackboard;
class EnemyAI;

class HNode
{
public:
    HNode(const std::string& name) : m_Name(name), m_Parent(nullptr), m_Status(NodeStatus::RUNNING) {}
    virtual ~HNode() = default;
    
    NodeStatus Tick();
    virtual void OnStart();
    virtual NodeStatus Update() = 0;
    virtual void OnFinished();
    virtual void OnAbort();
    
    virtual void AddChild(std::unique_ptr<HNode> child);
    virtual void AddConditionNode(std::unique_ptr<HCondition> conditionNode);

    virtual bool CanStart() { return true; }
    
    const HNode* GetParent() const { return m_Parent; }
    const NodeStatus GetStatus() const { return m_Status; }
    std::string GetName() const { return m_Name; }
    std::vector<std::unique_ptr<HNode>>& GetChildrens() { return m_Childrens; }
    std::vector<std::unique_ptr<HCondition>>& GetConditionNodes() { return m_ConditionNodes; }
    HNode* m_Parent;
protected:
    std::string m_Name;
    NodeStatus m_Status;
    bool m_bIsStarted = false;
    std::vector<std::unique_ptr<HNode>> m_Childrens;
    std::vector<std::unique_ptr<HCondition>> m_ConditionNodes;
};

class HRootNode : public HNode
{
public:
    HRootNode() : HNode("Root") {}

    virtual void OnStart() override;
    virtual NodeStatus Update() override;
    virtual void OnFinished() override;
    virtual void OnAbort() override;

    void AddChild(std::unique_ptr<HNode> child) override;
    void AddConditionNode(std::unique_ptr<HCondition> conditionNode) override {}
};

class HActionNode : public HNode
{
public:
    HActionNode(const std::string& name) : HNode(name), m_Owner(nullptr), m_Blackboard(nullptr) {}
    
    virtual void OnStart() override;
    virtual NodeStatus Update() override;
    virtual void OnFinished() override;
    virtual void OnAbort() override;

    bool CanStart() override;
    
    bool CheckConditions();
    bool CheckConditionsSelfMode();
protected:
    EnemyAI& GetOwner() const { return *m_Owner; }
    HBlackboard& GetBlackboard() const { return *m_Blackboard; }
private:
    EnemyAI* m_Owner;
    HBlackboard* m_Blackboard;
    
    void SetOwner(EnemyAI* owner) { m_Owner = owner; }
    void SetBlackboard(HBlackboard* blackboard) { m_Blackboard = blackboard; }
    friend class BehaviorTreeBuilder;
    friend class BehaviorTree;
};

class HCondition : public HNode
{
public:
    HCondition(const std::string& name) : HNode(name), m_Owner(nullptr), m_Blackboard(nullptr), m_PriortyMode(PriortyType::None) {}

    virtual void OnStart() override {}
    virtual NodeStatus Update() override = 0;
    virtual void OnFinished() override { m_bIsStarted = false; }
    virtual void OnAbort() override { HNode::OnAbort(); }

    PriortyType GetPriortyMode() const { return m_PriortyMode; }
protected:
    EnemyAI& GetOwner() const { return *m_Owner; }
    HBlackboard& GetBlackboard() const { return *m_Blackboard; }
private:
    EnemyAI* m_Owner;
    HBlackboard* m_Blackboard;
    PriortyType m_PriortyMode;
    
    void SetOwner(EnemyAI* owner) { m_Owner = owner; }
    void SetBlackboard(HBlackboard* blackboard) { m_Blackboard = blackboard; }
    void SetPriortyMode(PriortyType priorty) { m_PriortyMode = priorty; }
    friend class BehaviorTreeBuilder;
    friend class BehaviorTree;
};

class HDecorator : public HNode
{
public:
    HDecorator(const std::string& name) : HNode(name), m_Blackboard(nullptr), m_Owner(nullptr) {}

    virtual void OnStart() override {}
    virtual NodeStatus Update() override = 0;
    virtual void OnFinished() override { m_bIsStarted = false; }
    virtual void OnAbort() override { HNode::OnAbort(); }
protected:
    EnemyAI& GetOwner() const { return *m_Owner; }
    HBlackboard& GetBlackboard() const { return *m_Blackboard; }
private:
    EnemyAI* m_Owner;
    HBlackboard* m_Blackboard;

    void SetOwner(EnemyAI* owner) { m_Owner = owner; }
    void SetBlackboard(HBlackboard* blackboard) { m_Blackboard = blackboard; }
    friend class BehaviorTreeBuilder;
    friend class BehaviorTree;
};