#pragma once
#include "NodeEditorStructsAndEnums.h"

class NodeEditorApp
{
public:
    static void OnStart();
    static void Update();
private:
    static void BuildBehaviorTree();
};
