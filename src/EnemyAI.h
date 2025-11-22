#pragma once

class BehaviorTree;

class EnemyAI
{
public:
    EnemyAI();
    ~EnemyAI();

    void BeginPlay();
    void Tick(float DeltaTime);
    void EndPlay();
private:
    BehaviorTree* m_BehaviorTree;
    float m_TickTimer = 0.0f;
    bool b_StopAI = false;
};
