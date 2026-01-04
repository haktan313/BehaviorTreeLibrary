#pragma once
#include <memory>
#include <vector>
#include "Editor/NodeEditorApp.h"

class EnemyAI;
class BehaviorTree;

class Root
{
public:
    static void RootStart();
    static void RootTick();
    static void RootStop();
    static void BuildEditor();

    static BehaviorTree* CreateBehaviorTree(/*EnemyAI* owner*/);
    static void DestroyBehaviorTree(BehaviorTree* tree);
private:
    static std::vector<BehaviorTree*> m_BehaviorTrees;
    static std::unique_ptr<NodeEditorApp> m_NodeEditorApp;
};
