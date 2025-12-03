#pragma once
#include "NodeEditorStructsAndEnums.h"

class NodeEditorApp
{
public:
    NodeEditorApp();
    ~NodeEditorApp();

    static void Update();
private:
    static void BuildBehaviorTree();
    
    static NodeEditorApp* s_Instance;
};
