#include "Nodes.h"

#include <iostream>

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

void HNode::OnFinished()
{
    for (auto& child : m_Childrens)
    {
        if (child->GetStatus() == NodeStatus::RUNNING)
            child->OnFinished();
    }
    m_bIsStarted = false;
}

void HNode::OnAbort()
{
    std::cout << "Node Aborted: " << m_Name << std::endl;
    for (auto& child : m_Childrens)
    {
        if (child->GetStatus() == NodeStatus::RUNNING)
            child->OnAbort();
    }
    m_bIsStarted = false;
    m_Status = NodeStatus::FAILURE;
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

void HRootNode::OnStart()
{
    std::cout << "Root Node Started" << std::endl;
}

NodeStatus HRootNode::Update()
{
    while (m_bIsStarted)
    {
        NodeStatus status = m_Childrens.back()->Tick();
        if (status == NodeStatus::RUNNING)
            return NodeStatus::RUNNING;
        if (status == NodeStatus::FAILURE)
        {
            m_bIsStarted = false;
            return NodeStatus::FAILURE;
        }
        if (status == NodeStatus::SUCCESS)
        {
            m_bIsStarted = false;
            break;
        }
    }
    return NodeStatus::SUCCESS;
}

void HRootNode::OnFinished()
{
    m_bIsStarted = false;
    std::cout << "Root Node Finished with result: " << (m_Status == NodeStatus::SUCCESS ? "SUCCESS" : "FAILURE") << std::endl;
}

void HRootNode::OnAbort()
{
    std::cout << "Root Node Aborted" << std::endl;
    if (!m_Childrens.empty())
        if (m_Childrens.back()->GetStatus() == NodeStatus::RUNNING)
            m_Childrens.back()->OnAbort();
    m_bIsStarted = false;
    m_Status = NodeStatus::FAILURE;
}

void HRootNode::AddChild(std::unique_ptr<HNode> child)
{
    child->m_Parent = this;
    if (m_Childrens.empty())
        m_Childrens.push_back(std::move(child));
    else
        m_Childrens[0] = std::move(child);
}
