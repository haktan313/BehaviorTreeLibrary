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
    BehaviorTree* m_BehaviorTree;
private:
    float m_TickTimer = 0.0f;
    bool b_StopAI = false;
};
