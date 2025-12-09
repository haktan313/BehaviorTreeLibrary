#pragma once
#include <memory>
#include <string>
#include <vector>
#include "EnumsStructsForTree.h"
#include "BlackboardBase.h"

class HCondition;
class EnemyAI;
class NodeEditorApp;

class HNode
{
public:
    HNode(const std::string& name)
        : m_Name(name), m_Parent(nullptr), m_Status(NodeStatus::FAILURE), m_EditorApp(nullptr), m_Type(HNodeType::None) {}
    virtual ~HNode() = default;
    
    NodeStatus Tick();
    virtual void OnStart();
    virtual NodeStatus Update() = 0;
    virtual void OnFinished();
    virtual void OnAbort();
    
    virtual void AddChild(std::unique_ptr<HNode> child);
    virtual void AddConditionNode(std::unique_ptr<HCondition> conditionNode);

    virtual bool CanStart() { return true; }
    
    void SetParent(HNode* parent) { m_Parent = parent; }
    void SetEditorApp(NodeEditorApp* app) { m_EditorApp = app; }
    void SetType(HNodeType type) { m_Type = type; }
    
    NodeEditorApp* GetEditorApp() const { return m_EditorApp; }
    HNode* GetParent() const { return m_Parent; }
    NodeStatus GetStatus() const { return m_Status; }
    HNodeType GetType() const { return m_Type; }
    
    const std::string& GetName() const { return m_Name; }
    const std::vector<std::unique_ptr<HNode>>& GetChildrens() const { return m_Childrens; }
    const std::vector<std::unique_ptr<HCondition>>& GetConditionNodes() const { return m_ConditionNodes; }

    bool m_bIsStarted = false;
protected:
    const std::string m_Name;
    HNode* m_Parent;
    NodeEditorApp* m_EditorApp;
    NodeStatus m_Status;
    HNodeType m_Type;
    std::vector<std::unique_ptr<HNode>> m_Childrens;
    std::vector<std::unique_ptr<HCondition>> m_ConditionNodes;
};

class HRootNode : public HNode
{
public:
    HRootNode() : HNode("Root") {}

    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;

    void AddChild(std::unique_ptr<HNode> child) override;
    void AddConditionNode(std::unique_ptr<HCondition> conditionNode) override {}
};


struct ParamsForAction : Params
{
    ParamsForAction() = default;
    ~ParamsForAction() = default;

    virtual void DrawImGui(HBlackboard* blackboard) override {}
};
class HActionNode : public HNode
{
public:
    HActionNode(const std::string& name, const ParamsForAction& params = ParamsForAction{}) : HNode(name), m_Owner(nullptr), m_Blackboard(nullptr) {}
    
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

struct ParamsForCondition : Params
{
    ParamsForCondition() = default;
    ~ParamsForCondition() = default;

    virtual void DrawImGui(HBlackboard* blackboard) override {}

    PriorityType Priority = PriorityType::None;
};
class HCondition : public HNode
{
public:
    HCondition(const std::string& name, const ParamsForCondition& params = ParamsForCondition{})
        : HNode(name), m_Owner(nullptr), m_Blackboard(nullptr), m_PriorityMode(PriorityType::None), m_LastStatus(NodeStatus::RUNNING) {}

    virtual void OnStart() override {}
    virtual bool CheckCondition() = 0;
    virtual void OnFinished() override { m_bIsStarted = false; }
    virtual void OnAbort() override { HNode::OnAbort(); }

    void SetLastStatus(NodeStatus status) { m_LastStatus = status; }

    PriorityType GetPriorityMode() const { return m_PriorityMode; }
    NodeStatus GetLastStatus() const { return m_LastStatus; }
protected:
    EnemyAI& GetOwner() const { return *m_Owner; }
    HBlackboard& GetBlackboard() const { return *m_Blackboard; }
private:
    EnemyAI* m_Owner;
    HBlackboard* m_Blackboard;
    PriorityType m_PriorityMode;
    NodeStatus m_LastStatus;

    NodeStatus Update() override final { return CheckCondition() ? NodeStatus::SUCCESS : NodeStatus::FAILURE; }
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

    virtual void DrawImGui(HBlackboard* blackboard) override {}
}; 
class HDecorator : public HNode
{
public:
    HDecorator(const std::string& name, const ParamsForDecorator& params = ParamsForDecorator{}) : HNode(name), m_Blackboard(nullptr), m_Owner(nullptr) {}

    virtual void OnStart() override {}
    virtual bool CanExecute() = 0;
    virtual void OnFinishedResult(NodeStatus& status) = 0;
    virtual void OnFinished() override { m_bIsStarted = false; }
    virtual void OnAbort() override { HNode::OnAbort(); }
protected:
    EnemyAI& GetOwner() const { return *m_Owner; }
    HBlackboard& GetBlackboard() const { return *m_Blackboard; }
private:
    EnemyAI* m_Owner;
    HBlackboard* m_Blackboard;

    virtual NodeStatus Update() override final;
    void SetOwner(EnemyAI* owner) { m_Owner = owner; }
    void SetBlackboard(HBlackboard* blackboard) { m_Blackboard = blackboard; }
    friend class BehaviorTreeBuilder;
    friend class BehaviorTree;
};