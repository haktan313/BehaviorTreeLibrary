#pragma once
#include <unordered_map>

#include "EnemyAI.h"
#include "NodeEditorStructsAndEnums.h"

class NodeEditorApp
{
public:
    static void OnStart();
    static void Update();
    static void SetEnemyAI(EnemyAI* enemy) { m_Enemy = enemy; }
    static void AddActiveNode(HNode* node) { m_ActiveNodes.push_back(node); }
    static void RemoveActiveNode() { m_ActiveNodes.pop_back(); }
    static void ClearActiveNodes() { m_ActiveNodes.clear(); }

    static void ClearNodeMappings();
    static void RegisterNodeMapping(const HNode* runtimeNode, nodeEditor::NodeId editorId);
    static Node* GetEditorNodeFor(const HNode* runtimeNode);
private:
    static void BuildBehaviorTree();
    static EnemyAI* m_Enemy;
    static Node* m_LastHoveredNode;
    static BehaviorTree* m_BehaviorTree;
    static std::vector<HNode*> m_ActiveNodes;
    static std::unordered_map<const HNode*, nodeEditor::NodeId> s_NodeToEditorIdMap;
};
