#include "CompositeNodes.h"

#include <iostream>

#include "Editor/NodeEditorApp.h"

// HCompositeNode methods
bool HCompositeNode::CheckConditions()
{
    if (!m_ConditionNodes.empty())
        for (auto& condition : m_ConditionNodes)
        {
            NodeStatus conditionStatus = condition->Tick();
            if (conditionStatus == NodeStatus::FAILURE)
            {
                m_Status = NodeStatus::FAILURE;
                std::cout << "Node Condition Failed: " << condition->GetName() << " in " << m_Name << std::endl;
                return false;
            }
        }
    
    return true;
}

bool HCompositeNode::CanStart()
{
    if (!CheckConditions())
    {
        m_bIsStarted = false;
        m_Status = NodeStatus::FAILURE;
        return false;
    }
    return true;
}

// SequenceNode methods
void SequenceNode::OnStart()
{
    m_EditorApp->AddActiveNode(this);
    std::cout << "Sequence Node Started: " << m_Name << " - Parent:" << (m_Parent != nullptr ? m_Parent->GetName() : std::string(" NoParent")) << std::endl;
}

NodeStatus SequenceNode::Update()
{
    if (!m_EditorApp->CheckConditionsSelfMode(this, m_ConditionNodes))
        return NodeStatus::FAILURE;
    
    while (m_CurrentChildIndex < static_cast<int>(m_Childrens.size()))
    {
        NodeStatus status = m_Childrens[m_CurrentChildIndex]->Tick();
        if (status == NodeStatus::RUNNING)
            return NodeStatus::RUNNING;
        if (status == NodeStatus::FAILURE)
        {
            m_CurrentChildIndex = 0;
            return NodeStatus::FAILURE;
        }
        m_CurrentChildIndex++;
    }
    m_CurrentChildIndex = 0;
    return NodeStatus::SUCCESS;
}

void SequenceNode::OnFinished()
{
    std::cout << "Sequence Node Finished: " << m_Name << " result is: " << (m_Status == NodeStatus::SUCCESS ? "SUCCESS" : "FAILURE") << std::endl;
    m_CurrentChildIndex = 0;
    m_bIsStarted = false;
    m_EditorApp->RemoveActiveNode(this);
}

void SequenceNode::OnAbort()
{
    std::cout << "Sequence Node Aborted: " << m_Name << std::endl;
    if (m_CurrentChildIndex < static_cast<int>(m_Childrens.size()))
        if (m_Childrens[m_CurrentChildIndex]->GetStatus() == NodeStatus::RUNNING)
            m_Childrens[m_CurrentChildIndex]->OnAbort();
    m_CurrentChildIndex = 0;
    m_bIsStarted = false;
    m_Status = NodeStatus::FAILURE;
}

//SelectorNode methods
void SelectorNode::OnStart()
{
    m_EditorApp->AddActiveNode(this);
    std::cout << "Selector Node Started: " << m_Name << " - Parent:" << (m_Parent != nullptr ? m_Parent->GetName() : std::string(" NoParent")) << std::endl;
}

NodeStatus SelectorNode::Update()
{
    if (!m_EditorApp->CheckConditionsSelfMode(this, m_ConditionNodes))
        return NodeStatus::FAILURE;
    m_EditorApp->CheckConditionsLowerPriorityMode(m_CurrentChildIndex, this, m_Childrens);
    while (m_CurrentChildIndex < static_cast<int>(m_Childrens.size()))
    {
        NodeStatus status = m_Childrens[m_CurrentChildIndex]->Tick();
        if (status == NodeStatus::RUNNING)
            return NodeStatus::RUNNING;
        if (status == NodeStatus::FAILURE)
        {
            m_CurrentChildIndex++;
            continue;
        }
        if (status == NodeStatus::SUCCESS)
        {
            m_CurrentChildIndex = 0;
            return NodeStatus::SUCCESS;
        }
    }
    m_CurrentChildIndex = 0;
    return NodeStatus::FAILURE;
}

void SelectorNode::OnFinished()
{
    std::cout << "Selector Node Finished: " << m_Name << " result is: " << (m_Status == NodeStatus::SUCCESS ? "SUCCESS" : "FAILURE") << std::endl;
    m_CurrentChildIndex = 0;
    m_bIsStarted = false;
    m_EditorApp->RemoveActiveNode(this);
}

void SelectorNode::OnAbort()
{
    std::cout << "Selector Node Aborted: " << m_Name << std::endl;
    if (m_CurrentChildIndex < static_cast<int>(m_Childrens.size()))
        if (m_Childrens[m_CurrentChildIndex]->GetStatus() == NodeStatus::RUNNING)
            m_Childrens[m_CurrentChildIndex]->OnAbort();
    m_CurrentChildIndex = 0;
    m_bIsStarted = false;
    m_Status = NodeStatus::FAILURE;
}