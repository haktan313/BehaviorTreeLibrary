#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Nodes.h"
#include "BlackboardBase.h"
#include "Root.h"

class EnemyAI;

class BehaviorTree
{
public:
    BehaviorTree(EnemyAI* owner) : m_Owner(owner), m_Blackboard(nullptr) {}
    ~BehaviorTree();

    void StartTree();
    void TickTree();
    void StopTree();
    
    void SetRootNode(std::unique_ptr<HNode> root) { m_RootNode = std::move(root); }
    const HNode* GetRootNode() const { return m_RootNode.get(); }

private:
    bool m_bOwnsBlackboard = false;
    bool m_bIsRunning = false;
    EnemyAI* m_Owner;
    std::unique_ptr<HNode> m_RootNode;
    HBlackboard* m_Blackboard;

    friend class BehaviorTreeBuilder;
};

class BehaviorTreeBuilder
{
public:
    BehaviorTreeBuilder(EnemyAI* owner) : m_Tree(Root::CreateBehaviorTree(owner)) {}

    template<typename BlackboardType>
    BehaviorTreeBuilder& setBlackboard()
    {
        static_assert(std::is_base_of_v<HBlackboard, BlackboardType>, "BlackboardType must derive from HBlackboard");
        auto blackboard = new BlackboardType();
        m_Tree->m_Blackboard = blackboard;
        m_Tree->m_bOwnsBlackboard = true;
        return *this;
    }
    BehaviorTreeBuilder& setBlackboard(HBlackboard* blackboard)
    {
        m_Tree->m_Blackboard = blackboard;
        m_Tree->m_bOwnsBlackboard = false;
        return *this;
    }
    BehaviorTreeBuilder& root(NodeEditorApp* editorApp);
    BehaviorTreeBuilder& sequence(const std::string& name);
    BehaviorTreeBuilder& selector(const std::string& name);
    template<typename ActionNodeType, typename... Args>
    BehaviorTreeBuilder& action(Args&&... args)
    {
        static_assert(std::is_base_of_v<HActionNode, ActionNodeType>, "ActionNodeType must derive from HAction");
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
                auto editorApp = decoratorNodePtr->GetParent()->GetEditorApp();
                if (editorApp == nullptr)
                    std::cout << "editor app null" << std::endl;
                m_LastCreatedNode->SetEditorApp(editorApp);
            }
        }
        else
            if (!m_NodeStack.empty())
            {
                action->SetOwner(m_Tree->m_Owner);
                action->SetType(HNodeType::Action);
                action->SetBlackboard(m_Tree->m_Blackboard);
                m_NodeStack.back()->AddChild(std::move(action));
                auto editorApp = m_LastCreatedNode->GetParent()->GetEditorApp();
                if (editorApp == nullptr)
                    std::cout << "editor app null" << std::endl;
                m_LastCreatedNode->SetEditorApp(editorApp);
            }
        return *this;
    }
    template<typename ConditionNodeType, typename... Args>
    BehaviorTreeBuilder& condition(PriorityType priority, Args&&... args)
    {
        static_assert(std::is_base_of_v<HCondition, ConditionNodeType>, "ConditionNodeType must derive from HCondition");
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
        static_assert(std::is_base_of_v<HDecorator, DecoratorNodeType>, "DecoratorNodeType must derive from HDecorator");
        m_CurrentDecorator = std::make_unique<DecoratorNodeType>(std::forward<Args>(args)...);
        std::cout << "Adding Decorator Node: " << m_CurrentDecorator->GetName() << std::endl;
        m_CurrentDecorator->SetOwner(m_Tree->m_Owner);
        m_CurrentDecorator->SetBlackboard(m_Tree->m_Blackboard);
        m_CurrentDecorator->SetType(HNodeType::Decorator);
        return *this;
    }
    BehaviorTreeBuilder& end();
    BehaviorTree* build() const;

    const HNode* GetLastCreatedNode() const { return m_LastCreatedNode; }
private:
    BehaviorTree* m_Tree;
    HNode* m_LastCreatedNode = nullptr;
    std::unique_ptr<HDecorator> m_CurrentDecorator;
    std::vector<HNode*> m_NodeStack;
};