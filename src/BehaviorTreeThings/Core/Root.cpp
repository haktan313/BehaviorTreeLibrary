#include "Root.h"

#include "Tree.h"

std::vector<BehaviorTree*> Root::m_BehaviorTrees;

void Root::RootStart()
{
    std::cout << "Root Started" << std::endl;
    m_BehaviorTrees.clear();
}

void Root::RootTick()
{
    for (BehaviorTree* tree : m_BehaviorTrees)
        tree->TickTree();
}

void Root::RootStop()
{
    std::cout << "Root Stopped" << std::endl;
    for (BehaviorTree* tree : m_BehaviorTrees)
    {
        tree->StopTree();
        delete tree;
    }
    m_BehaviorTrees.clear();
}

BehaviorTree* Root::CreateBehaviorTree(EnemyAI* owner)
{
    BehaviorTree* tree = new BehaviorTree(owner);
    m_BehaviorTrees.push_back(tree);
    return tree;
}

void Root::DestroyBehaviorTree(BehaviorTree* tree)
{
    if (tree)
    {
        tree->StopTree();
        auto it = std::find(m_BehaviorTrees.begin(), m_BehaviorTrees.end(), tree);
        if (it != m_BehaviorTrees.end())
            m_BehaviorTrees.erase(it);
        delete tree;
    }
}