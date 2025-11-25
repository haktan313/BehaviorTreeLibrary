#include "CustomNodes.h"
#include <iostream>
#include "../Core/Tree.h"


//ActionNode methods
void ActionNode::OnStart()
{
    std::cout << "Action Node Started: " << m_Name << " - Parent:" << (m_Parent != nullptr ? m_Parent->GetName() : std::string(" NoParent")) << std::endl;
    std::cout << "Action Node Speed: " << m_Speed << std::endl;
}

NodeStatus ActionNode::Update()
{
    if (m_TickCount < 20.0f)
    {
        //std::cout << "Parent Sequence Name:" << m_Parent->GetName() << " Action Node Running: " << m_Name << " (Tick " << m_TickCount + 0.1f << ")" << std::endl;
        m_TickCount += 0.1f;
        return NodeStatus::RUNNING;
    }
    m_TickCount = 0;
    std::cout << "Action Node Succeeded: " << m_Name << std::endl;
    return NodeStatus::SUCCESS;
}

void ActionNode::OnFinished()
{
    std::cout << "Action Node Finished: " << m_Name << std::endl;
    m_TickCount = 0;
    m_bIsStarted = false;
}

void ActionNode::OnAbort()
{
    std::cout << "Action Node Aborted: " << m_Name << std::endl;
    m_TickCount = 0;
    m_bIsStarted = false;
    m_Status = NodeStatus::FAILURE;
}

NodeStatus CanSeePlayerCondition::Update()
{
    return GetBlackboard().GetBoolValue("IsPlayerVisible") ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus InverterDecorator::Update()
{
    HNode* child = m_Childrens.empty() ? nullptr : m_Childrens[0].get();
    if (!child)
        return NodeStatus::FAILURE;
    NodeStatus status = child->Tick();
    switch (status)
    {
        case NodeStatus::SUCCESS:
            std::cout << "Inverter Decorator: " << m_Name << " inverts SUCCESS to FAILURE. The Node was:" << child->GetName() << std::endl;
            return NodeStatus::FAILURE;
        case NodeStatus::FAILURE:
            std::cout << "Inverter Decorator: " << m_Name << " inverts FAILURE to SUCCESS The Node was:" << child->GetName() << std::endl;
            return NodeStatus::SUCCESS;
        case NodeStatus::RUNNING:
            return NodeStatus::RUNNING;
        default:
            return NodeStatus::FAILURE;
    }
}
