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
    
    /*m_BehaviorTree = BehaviorTreeBuilder(this).setBlackboard<EnemyBlackboard>()
    .root()
    .selector("RootNode")
        /*.sequence("PatrolSequence").condition<CanSeePlayerCondition>(PriortyType::Both,"CanSeePlayerCondition",true)
            .sequence("PatrolSubSequence")
                .action<ActionNode>("MoveToPatrolPointAction", ActionNodeParams{1.f})
                //.decorator<InverterDecorator>("Inverter_RotateAtPatrolPoint")
                .action<ActionNode>("RotateAtPatrolPointAction", ActionNodeParams{69.f})
            .end()
            .action<ActionNode>("WaitAtPatrolPointAction", ActionNodeParams{31.f})
        .end()
        //.decorator<InverterDecorator>("Inverter_CanSeePlayer")
            .sequence("ChaseSequence")
                .action<ActionNode>("DetectPlayerAction", ActionNodeParams{10.f})
                .condition<CanSeePlayerCondition>(PriortyType::LowerPriority,"IsPlayerVisible",false)
                .action<ActionNode>("ChasePlayerAction", ActionNodeParams{2.f})  
            .end()#1#
        .decorator<InverterDecorator>("Inverter_CanSeePlayer", InverterDecoratorParams{})
        .action<ActionNode>("MoveToPatrolPointAction", ActionNodeParams{1.f})
        /*.condition<CanSeePlayerCondition>(PriortyType::Both,"CanSeePlayerCondition",false)#1#
        .action<ActionNode>("ChasePlayerAction", ActionNodeParams{2.f})
    .end()
    .build();
    
    m_BehaviorTree->StartTree();*/
}

void EnemyAI::Tick(float DeltaTime)
{
    /*if (m_BehaviorTree && !b_StopAI)
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
        m_BehaviorTree = nullptr;#1#
        m_BehaviorTree->GetBlackboard()->SetBoolValue("IsPlayerVisible", !m_BehaviorTree->GetBlackboard()->GetBoolValue("IsPlayerVisible"));
        b_StopAI = false;
    }*/
}

void EnemyAI::EndPlay()
{
    std::cout << "Enemy AI Ended" << std::endl;
}
