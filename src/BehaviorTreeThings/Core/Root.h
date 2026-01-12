#pragma once
#include <vector>

class BehaviorTree;

class Root
{
public:
    static void RootTick();
    static void RootClear();

    static BehaviorTree* CreateBehaviorTree();
    static void DestroyBehaviorTree(BehaviorTree* tree);
private:
    static std::vector<BehaviorTree*> m_BehaviorTrees;
};
