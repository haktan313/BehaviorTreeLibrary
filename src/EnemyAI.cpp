#include "EnemyAI.h"
#include <iostream>

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
}

void EnemyAI::Tick(float DeltaTime)
{
}

void EnemyAI::EndPlay()
{
    std::cout << "Enemy AI Ended" << std::endl;
}
