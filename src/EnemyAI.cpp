#include "EnemyAI.h"
#include <iostream>

#include "BehaviorTreeThings/Core/Tree.h"
#include "BehaviorTreeThings/CustomThings/CustomBlackboard.h"
#include "BehaviorTreeThings/CustomThings/CustomNodes.h"

EnemyAI::EnemyAI()
{
    BeginPlay();
}

EnemyAI::~EnemyAI()
{
    EndPlay(); 
} 

void EnemyAI::BeginPlay()
{
    std::cout << "Enemy AI Started" << std::endl;
    
    m_BehaviorTree = BehaviorTreeBuilder(this).setBlackboard<EnemyBlackboard>()
    .root()
    .selector("RootNode")
        .sequence("PatrolSequence").condition<CanSeePlayerCondition>(PriortyType::Both,"CanSeePlayerCondition",false)
            .action<ActionNode>("MoveToPatrolPointAction",10.0f)
            .action<ActionNode>("WaitAtPatrolPointAction",5.0f)
        .end()
        .decorator<InverterDecorator>("Inverter_CanSeePlayer")
            .sequence("ChaseSequence")
                .action<ActionNode>("DetectPlayerAction",50.0f)
                .action<ActionNode>("ChasePlayerAction",40.0f)
            .end()
    .end()
    .build();
    
    m_BehaviorTree->StartTree();
}

void EnemyAI::Tick(float DeltaTime)
{
    if (m_BehaviorTree && !b_StopAI)
    {
        m_TickTimer += DeltaTime;
        if (m_TickTimer >= 5.0f)
        {
            b_StopAI = true;
            m_TickTimer = 0.0f;
        }
    }
    if (b_StopAI && m_BehaviorTree)
    {
        /*m_BehaviorTree->StopTree();
        m_BehaviorTree = nullptr;*/
        m_BehaviorTree->GetBlackboard()->SetBoolValue("IsPlayerVisible", !m_BehaviorTree->GetBlackboard()->GetBoolValue("IsPlayerVisible"));
        b_StopAI = false;
    }
}

void EnemyAI::EndPlay()
{
    std::cout << "Enemy AI Ended" << std::endl;
}
