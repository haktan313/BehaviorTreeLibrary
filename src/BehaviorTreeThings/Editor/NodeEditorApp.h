#pragma once
#include "EnemyAI.h"
#include "NodeEditorStructsAndEnums.h"

class NodeEditorApp
{
public:
    static void OnStart();
    static void Update();
    static void SetEnemyAI(EnemyAI* enemy) { m_Enemy = enemy; } 
private:
    static void BuildBehaviorTree();
    static EnemyAI* m_Enemy;
};
