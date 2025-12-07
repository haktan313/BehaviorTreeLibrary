#pragma once
#include <memory>
#include <string>
#include <vector>
#include "EnumsForTree.h"

class HCondition;
class HBlackboard;
class EnemyAI;
class NodeEditorApp;

class HNode
{
public:
    HNode(const std::string& name) : m_Name(name), m_Parent(nullptr), m_Status(NodeStatus::RUNNING), m_EditorApp(nullptr) {}
    virtual ~HNode() = default;
    
    NodeStatus Tick();
    virtual void OnStart();
    virtual NodeStatus Update() = 0;
    virtual void OnFinished();
    virtual void OnAbort();
    
    virtual void AddChild(std::unique_ptr<HNode> child);
    virtual void AddConditionNode(std::unique_ptr<HCondition> conditionNode);

    virtual bool CanStart() { return true; }
    
    HNode* GetParent() const { return m_Parent; }
    void SetParent(HNode* parent) { m_Parent = parent; }
    void SetEditorApp(NodeEditorApp* app) { m_EditorApp = app; }
    NodeEditorApp* GetEditorApp() { return m_EditorApp; }
    NodeStatus GetStatus() const { return m_Status; }
    std::string GetName() const { return m_Name; }
    std::vector<std::unique_ptr<HNode>>& GetChildrens() { return m_Childrens; }
    std::vector<std::unique_ptr<HCondition>>& GetConditionNodes() { return m_ConditionNodes; }
protected:
    HNode* m_Parent;
    NodeEditorApp* m_EditorApp;
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

struct Params
{
    Params() = default;
    ~Params() = default;
    
    virtual void DrawImGui() {}
};
class HActionNode : public HNode
{
public:
    HActionNode(const std::string& name, const Params& params = Params{}) : HNode(name), m_Owner(nullptr), m_Blackboard(nullptr) {}
    
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

struct ParamsForCondition : public Params
{
    ParamsForCondition() = default;
    ~ParamsForCondition() = default;

    virtual void DrawImGui() override {}

    PriorityType Priority = PriorityType::None;
};
class HCondition : public HNode
{
public:
    HCondition(const std::string& name, const ParamsForCondition& params = ParamsForCondition{}) : HNode(name), m_Owner(nullptr), m_Blackboard(nullptr), m_PriorityMode(PriorityType::None) {}

    virtual void OnStart() override {}
    virtual NodeStatus Update() override = 0;
    virtual void OnFinished() override { m_bIsStarted = false; }
    virtual void OnAbort() override { HNode::OnAbort(); }

    PriorityType GetPriorityMode() const { return m_PriorityMode; }
protected:
    EnemyAI& GetOwner() const { return *m_Owner; }
    HBlackboard& GetBlackboard() const { return *m_Blackboard; }
private:
    EnemyAI* m_Owner;
    HBlackboard* m_Blackboard;
    PriorityType m_PriorityMode;
    
    void SetOwner(EnemyAI* owner) { m_Owner = owner; }
    void SetBlackboard(HBlackboard* blackboard) { m_Blackboard = blackboard; }
    void SetPriorityMode(PriorityType priority) { m_PriorityMode = priority; }
    friend class BehaviorTreeBuilder;
    friend class BehaviorTree;
};

struct ParamsForDecorator : public Params
{
    ParamsForDecorator() = default;
    ~ParamsForDecorator() = default;

    virtual void DrawImGui() override {}
}; 
class HDecorator : public HNode
{
public:
    HDecorator(const std::string& name, const ParamsForDecorator& params = ParamsForDecorator{}) : HNode(name), m_Blackboard(nullptr), m_Owner(nullptr) {}

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