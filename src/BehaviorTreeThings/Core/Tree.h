#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Nodes.h"
#include "BlackboardBase.h"
#include "Root.h"

class EnemyAI;
class BehaviorTree;

class BehaviorTree
{
public:
    BehaviorTree(EnemyAI* owner) : m_Owner(owner), m_Blackboard(nullptr) {}
    ~BehaviorTree();

    void StartTree();
    void TickTree();
    void StopTree();
    
    void SetRootNode(std::unique_ptr<HNode> root) { m_RootNode = std::move(root); }
    HNode* GetRootNode() const { return m_RootNode.get(); }

private:
    std::unique_ptr<HNode> m_RootNode;
    EnemyAI* m_Owner;
    HBlackboard* m_Blackboard;
    bool m_bIsRunning = false;
    friend class BehaviorTreeBuilder;
};

class BehaviorTreeBuilder
{
public:
    BehaviorTreeBuilder(EnemyAI* owner) : m_Tree(Root::CreateBehaviorTree(owner)) {}

    template<typename BlackboardType>
    BehaviorTreeBuilder& setBlackboard()
    {
        auto blackboard = new BlackboardType();
        m_Tree->m_Blackboard = blackboard;
        return *this;
    }
    BehaviorTreeBuilder& setBlackboard(HBlackboard* blackboard)
    {
        m_Tree->m_Blackboard = blackboard;
        return *this;
    }
    BehaviorTreeBuilder& root(NodeEditorApp* editorApp);
    BehaviorTreeBuilder& sequence(const std::string& name);
    BehaviorTreeBuilder& selector(const std::string& name);
    template<typename ActionNodeType, typename... Args>
    BehaviorTreeBuilder& action(Args&&... args)
    {
        auto action = std::make_unique<ActionNodeType>(std::forward<Args>(args)...);
        std::cout << "Adding Action Node: " << action->GetName() << std::endl;
        m_LastCreatedNode = action.get();
        if (m_CurrentDecorator)
        {
            auto decoratorNode = std::move(m_CurrentDecorator);
            auto decoratorNodePtr = decoratorNode.get();
            if (!m_NodeStack.empty())
            {
                action->SetOwner(m_Tree->m_Owner);
                action->SetType(HNodeType::Action);
                action->SetBlackboard(m_Tree->m_Blackboard);
                decoratorNode->AddChild(std::move(action));
                m_NodeStack.back()->AddChild(std::move(decoratorNode));
                m_LastCreatedNode->SetEditorApp(decoratorNodePtr->GetParent()->GetEditorApp());
            }
        }
        else
        {
            if (!m_NodeStack.empty())
            {
                action->SetOwner(m_Tree->m_Owner);
                action->SetType(HNodeType::Action);
                action->SetBlackboard(m_Tree->m_Blackboard);
                m_NodeStack.back()->AddChild(std::move(action));
                m_LastCreatedNode->SetEditorApp(m_LastCreatedNode->GetParent()->GetEditorApp());
            }
        }
        return *this;
    }
    template<typename ConditionNodeType, typename... Args>
    BehaviorTreeBuilder& condition(PriorityType priority, Args&&... args)
    {
        auto condition = std::make_unique<ConditionNodeType>(std::forward<Args>(args)...);
        std::cout << "Adding Condition Node: " << condition->GetName() << std::endl;
        if (m_LastCreatedNode)
        {
            condition->SetOwner(m_Tree->m_Owner);
            condition->SetBlackboard(m_Tree->m_Blackboard);
            condition->SetPriorityMode(priority);
            condition->SetType(HNodeType::Condition);
            m_LastCreatedNode->AddConditionNode(std::move(condition));
        }
        return *this;
    }
    template<typename DecoratorNodeType, typename... Args>
    BehaviorTreeBuilder& decorator(Args&&... args)
    {
        m_CurrentDecorator = std::make_unique<DecoratorNodeType>(std::forward<Args>(args)...);
        std::cout << "Adding Decorator Node: " << m_CurrentDecorator->GetName() << std::endl;
        m_CurrentDecorator->SetOwner(m_Tree->m_Owner);
        m_CurrentDecorator->SetBlackboard(m_Tree->m_Blackboard);
        m_CurrentDecorator->SetType(HNodeType::Decorator);
        return *this;
    }
    BehaviorTreeBuilder& end();
    BehaviorTree* build();

    HNode* GetLastCreatedNode() const { return m_LastCreatedNode; }
private:
    BehaviorTree* m_Tree;
    std::vector<HNode*> m_NodeStack;
    std::unique_ptr<HDecorator> m_CurrentDecorator;
    HNode* m_LastCreatedNode = nullptr;
};