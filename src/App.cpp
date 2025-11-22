#include "App.h"
#include "BehaviorTreeThings/Core/Tree.h"

App::App()
{
    Root::RootStart();
    m_EnemyAI = new EnemyAI();
}

App::~App()
{
    Root::RootStop();
    delete m_EnemyAI;
    m_EnemyAI = nullptr;
}

void App::Run()
{
    while (m_EnemyAI != nullptr)
    {
        Root::RootTick();
        m_EnemyAI->Tick(0.016f);
    }
}
