#include "Nodes.h"

// HNode methods
NodeStatus HNode::Tick()
{
    if (!m_bIsStarted)
    {
        m_bIsStarted = true;
        OnStart();
    }
    if (!m_bIsStarted)
        return m_Status;
    NodeStatus status = Update();
    m_Status = status;
    if (status == NodeStatus::SUCCESS || status == NodeStatus::FAILURE)
        OnFinished();
    return status;
}

void HNode::AddChild(std::unique_ptr<HNode> child)
{
    child->m_Parent = this;
    m_Childrens.push_back(std::move(child));
}

void HNode::AddConditionNode(std::unique_ptr<HCondition> conditionNode)
{
    conditionNode->m_Parent = this;
    m_ConditionNodes.push_back(std::move(conditionNode));
}
