#include "Root.h"
#include "Tree.h"
#include "Editor/NodeEditorApp.h"

std::vector<BehaviorTree*> Root::m_BehaviorTrees;
std::unique_ptr<NodeEditorApp> Root::m_NodeEditorApp;

void Root::RootStart()
{
    std::cout << "Root Started" << std::endl;
    if (m_NodeEditorApp)
        m_NodeEditorApp->OnStart();
}

void Root::RootTick()
{
    if (m_NodeEditorApp)
        m_NodeEditorApp->Update();
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

void Root::BuildEditor()
{
    m_NodeEditorApp = std::make_unique<NodeEditorApp>();
    //m_NodeEditorApp->SetEnemyAI(m_EnemyAI);
}

BehaviorTree* Root::CreateBehaviorTree(/*EnemyAI* owner*/)
{
    BehaviorTree* tree = new BehaviorTree(/*owner*/);
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