#include <iostream>
#include "Nodes.h"
#include "Editor/NodeEditorApp.h"

// HNode methods
NodeStatus HNode::Tick()
{
    if (!m_bIsStarted)
    {
        if (!CanStart())
        {
            m_Status = NodeStatus::FAILURE;
            return m_Status;
        }
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

void HNode::OnStart()
{
    m_EditorApp->AddActiveNode(this);
}

void HNode::OnFinished()
{
    m_EditorApp->RemoveActiveNode(this);
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
    m_EditorApp->RemoveActiveNode(this);
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
    m_EditorApp->AddActiveNode(this);
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
    m_EditorApp->RemoveActiveNode(this);
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
    //child->GetParent() = this;
    child->SetParent(this);
    if (m_Childrens.empty())
        m_Childrens.push_back(std::move(child));
    else
        m_Childrens[0] = std::move(child);
}

void HActionNode::OnStart()
{
    HNode::OnStart();
}

NodeStatus HActionNode::Update()
{
    return CheckConditionsSelfMode() ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

void HActionNode::OnFinished()
{
    HNode::OnFinished();
}

void HActionNode::OnAbort()
{
    HNode::OnAbort();
}

bool HActionNode::CanStart()
{
    if (!CheckConditions())
    {
        m_bIsStarted = false;
        m_Status = NodeStatus::FAILURE;
        return false;
    }
    return true;
}

bool HActionNode::CheckConditions()
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

bool HActionNode::CheckConditionsSelfMode()
{
    if (!m_ConditionNodes.empty())
        for (auto& condition : m_ConditionNodes)
        {
            if (condition->GetPriorityMode() == PriorityType::None)
                continue;
            NodeStatus conditionStatus = condition.get()->Tick();
            if ((condition->GetPriorityMode() == PriorityType::Self || condition->GetPriorityMode() == PriorityType::Both) && conditionStatus == NodeStatus::FAILURE)
            {
                OnAbort();
                std::cout << "Node Condition Failed at Runtime: " << condition->GetName() << " in " << m_Name << std::endl;
                return false;
            }
        }
    return true;
}

NodeStatus HDecorator::Update()
{
    if (!m_Childrens[0]->m_bIsStarted)
        if (!CanExecute())
            return NodeStatus::FAILURE;
    
    NodeStatus status = m_Childrens[0]->Tick();
    if (status != NodeStatus::RUNNING)
        OnFinishedResult(status);
    return status;
}
