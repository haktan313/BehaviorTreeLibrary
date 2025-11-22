#pragma once
#include <vector>

class EnemyAI;
class BehaviorTree;

class Root
{
public:
    static void RootStart();
    static void RootTick();
    static void RootStop();

    static BehaviorTree* CreateBehaviorTree(EnemyAI* owner);
    static void DestroyBehaviorTree(BehaviorTree* tree);
private:
    static std::vector<BehaviorTree*> m_BehaviorTrees;
};
