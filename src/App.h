#pragma once
#include "EnemyAI.h"

class App
{
public:
    App();
    ~App();
    void Run();
private:
    EnemyAI* m_EnemyAI;
};
