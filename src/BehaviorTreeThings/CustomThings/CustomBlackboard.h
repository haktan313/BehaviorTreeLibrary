#pragma once
#include "../Core/BlackboardBase.h"

class EnemyBlackboard : public HBlackboard
{
public:
    EnemyBlackboard();

    void DrawImGui() override;
};

class EnemySoilderBlackboard : public HBlackboard
{
public:
    EnemySoilderBlackboard();

    void DrawImGui() override;
};
