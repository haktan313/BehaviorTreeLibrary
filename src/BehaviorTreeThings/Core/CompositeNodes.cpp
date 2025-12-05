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

bool HCompositeNode::CheckConditionsSelfMode()
{
    if (!m_ConditionNodes.empty())
        for (auto& condition : m_ConditionNodes)
        {
            if (condition->GetPriortyMode() == PriortyType::None)
                continue;
            NodeStatus conditionStatus = condition.get()->Tick();
            if ((condition->GetPriortyMode() == PriortyType::Self || condition->GetPriortyMode() == PriortyType::Both) && conditionStatus == NodeStatus::FAILURE)
            {
                OnAbort();
                std::cout << "Node Condition Failed at Runtime: " << condition->GetName() << " in " << m_Name << std::endl;
                return false;
            }
        }
    return true;
}

void HCompositeNode::CheckConditionsLowerPriortyMode(int& currentChildIndex)
{
    if (!m_Childrens.empty())
        for (int i = 0; i < static_cast<int>(m_Childrens.size()); ++i)
        {
            if (i >= currentChildIndex)
                continue;
            auto& child = m_Childrens[i];
            for (auto& condition : child->GetConditionNodes())
            {
                if (condition->GetPriortyMode() == PriortyType::None)
                    continue;
                NodeStatus conditionStatus = condition.get()->Tick();
                if ((condition->GetPriortyMode() == PriortyType::LowerPriority || condition->GetPriortyMode() == PriortyType::Both) && conditionStatus == NodeStatus::SUCCESS)
                {
                    m_Childrens[currentChildIndex]->OnAbort();
                    currentChildIndex = i;
                    std::cout << "Node Condition Succeeded at Runtime: " << condition->GetName() << " in " << m_Name << std::endl;
                    return;
                }
            }
        }
}

// SequenceNode methods
void SequenceNode::OnStart()
{
    if (!CheckConditions())
    {
        m_bIsStarted = false;
        m_Status = NodeStatus::FAILURE;
        return;
    }
    NodeEditorApp::AddActiveNode(this);
    std::cout << "Sequence Node Started: " << m_Name << " - Parent:" << (m_Parent != nullptr ? m_Parent->GetName() : std::string(" NoParent")) << std::endl;
}

NodeStatus SequenceNode::Update()
{
    if (!CheckConditionsSelfMode())
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
    NodeEditorApp::RemoveActiveNode();
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
    if (!CheckConditions())
    {
        m_bIsStarted = false;
        m_Status = NodeStatus::FAILURE;
        return;
    }
    NodeEditorApp::AddActiveNode(this);
    std::cout << "Selector Node Started: " << m_Name << " - Parent:" << (m_Parent != nullptr ? m_Parent->GetName() : std::string(" NoParent")) << std::endl;
}

NodeStatus SelectorNode::Update()
{
    if (!CheckConditionsSelfMode())
        return NodeStatus::FAILURE;
    CheckConditionsLowerPriortyMode(m_CurrentChildIndex);
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
    NodeEditorApp::RemoveActiveNode();
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