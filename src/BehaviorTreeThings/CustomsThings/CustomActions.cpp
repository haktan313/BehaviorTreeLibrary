#include <iostream>
#include "CustomActions.h"
#include "BlackboardBase.h"

void MoveToAction::OnStart()
{
    HActionNode::OnStart();
    m_DistanceToTarget = GetBlackboard().GetFloatValue("DistanceToPlayer");
    std::cout << "MoveToAction started." << std::endl;
    std::cout << "Initial Distance to Target: " << m_DistanceToTarget << std::endl;
}

NodeStatus MoveToAction::Update()
{
    if (m_DistanceToTarget <= m_StopDistance)
    {
        std::cout << "Reached target. Current Distance: " << m_DistanceToTarget << std::endl;
        GetBlackboard().SetFloatValue("DistanceToPlayer", m_DistanceToTarget);
        return NodeStatus::SUCCESS;
    }
    m_DistanceToTarget -= m_MoveSpeed;
    GetBlackboard().SetFloatValue("DistanceToPlayer", m_DistanceToTarget);
    std::cout << "Moving towards target. Current Distance: " << m_DistanceToTarget << std::endl;
    return NodeStatus::RUNNING;
}

void MoveToAction::OnFinished()
{
    HActionNode::OnFinished();
    std::cout << "MoveToAction finished." << std::endl;
}

void MoveToAction::OnAbort()
{
    HActionNode::OnAbort();
    std::cout << "MoveToAction aborted." << std::endl;
}

//Melee Enemy Actions
//Melee Enemy Attack Action

void MeleeEnemyAttackAction::OnStart()
{
    HActionNode::OnStart();
    GetBlackboard().SetBoolValue("IsAttacking", true);
    std::cout << "MeleeEnemyAttackAction started." << std::endl;
}

NodeStatus MeleeEnemyAttackAction::Update()
{
    m_ElapsedTime += .1f;
    if (m_ElapsedTime >= m_AttackDuration)
    {
        std::cout << "MeleeEnemyAttackAction completed attack." << std::endl;
        int attackPower = GetBlackboard().GetIntValue(m_AttackPowerKey);
        std::cout << "Dealt " << attackPower << " damage to the player." << std::endl;
        GetBlackboard().SetBoolValue("IsAttacking", false);
        m_ElapsedTime = 0.0f;
        return NodeStatus::SUCCESS;
    }
    std::cout << "MeleeEnemyAttackAction attacking... Elapsed Time: " << m_ElapsedTime << std::endl;
    return NodeStatus::RUNNING;
}

void MeleeEnemyAttackAction::OnFinished()
{
    HActionNode::OnFinished();
    m_ElapsedTime = 0.0f;
    std::cout << "MeleeEnemyAttackAction finished." << std::endl;
}

void MeleeEnemyAttackAction::OnAbort()
{
    HActionNode::OnAbort();
    GetBlackboard().SetBoolValue("IsAttacking", false);
    m_ElapsedTime = 0.0f;
    std::cout << "MeleeEnemyAttackAction aborted." << std::endl;
}

