#include "EnemyAI.h"
#include <iostream>

#include "App.h"
#include "BTSerializer.h"
#include "PlatformUtilsBT.h"
#include "Tree.h"
#include "CustomThings/CustomActions.h"
#include "CustomThings/CustomBlackboards.h"
#include "CustomThings/CustomConditions.h"
#include "CustomThings/CustomDecorators.h"

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
    /*MoveToParameters moveToParams;
    moveToParams.bToPlayer = true;
    moveToParams.MoveSpeed = 5.0f;
    moveToParams.StopDistance = 15.0f;
    
    MeleeEnemyAttackActionParameters attackParams;
    attackParams.AttackDuration = 1.f;
    attackParams.AttackPowerKey = "AttackPower";
    
    ChangeResultOfTheNodeParameters changeResultOfTheNodeParams;
    changeResultOfTheNodeParams.NewResult = NodeStatus::FAILURE;
    
    IsPlayerInRangeParameters isPlayerInRangeParams;
    isPlayerInRangeParams.Range = 31.f;
    isPlayerInRangeParams.DistanceToPlayerKey = "DistanceToPlayer";
    isPlayerInRangeParams.Priority = PriorityType::Both;
    
    CooldownDecoratorParameters cooldownDecoratorParams;
    cooldownDecoratorParams.CooldownTime = 5.f;
    
    m_BehaviorTree = BehaviorTreeBuilder(this).setBlackboard<MeleeEnemyBlackboard>()
        .root(App::GetNodeEditorApp())
            .sequence("Main Sequence")
                .decorator<CooldownDecorator>("Cooldown Decorator", cooldownDecoratorParams).action<MeleeEnemyAttackAction>("Melee Attack Action", attackParams)
                    .condition<IsPlayerInRangeCondition>(PriorityType::Both, "Is Player In Range", isPlayerInRangeParams)
                .decorator<ChangeResultOfTheNodeDecorator>("Change Result Of The Node", changeResultOfTheNodeParams)
                    .action<MoveToAction>("Move To Player Action", moveToParams)
            .end()
        .build();
    m_BehaviorTree->StartTree();*/
    BehaviorTree* tree = nullptr;
    auto btPath = PlatformUtilsBT::OpenFile("Behavior Tree Files\0*.btree\0");
    BTSerializer serializer(*tree);
    if (serializer.Deserialize(btPath))
    {
        m_BehaviorTree = tree;
        m_BehaviorTree->StartTree();
    }
        
}

void EnemyAI::Tick(float DeltaTime)
{
}

void EnemyAI::EndPlay()
{
    std::cout << "Enemy AI Ended" << std::endl;
}
