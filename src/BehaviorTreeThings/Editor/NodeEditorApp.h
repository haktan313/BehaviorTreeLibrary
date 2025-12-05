#pragma once
#include <functional>
#include <unordered_map>
#include "Tree.h"
#include "NodeEditorStructsAndEnums.h"

struct ActionClassInfo
{
    std::string NameAsID;
    std::string Name;
    std::function<void(BehaviorTreeBuilder&, Node*, Params&)> BuildFn;
    std::function<std::unique_ptr<Params>()> CreateParamsFn;
};

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
    static void BlackboardPanel();
    static void BuildBehaviorTree();
    static void BuildPlanForNode(Node* editorNode, std::vector<BuildOp>& ops);
    static std::vector<BuildOp> CreateBuildPlan();
    
    static EnemyAI* m_Enemy;
    static Node* m_LastHoveredNode;
    static Node* m_LastSelectedNode;
    static BehaviorTree* m_BehaviorTree;
    static std::vector<HNode*> m_ActiveNodes;
    static std::unordered_map<const HNode*, nodeEditor::NodeId> s_NodeToEditorIdMap;
    
    static std::unordered_map<std::string, ActionClassInfo> s_ActionClassInfoMap;
    static std::unordered_map<int, std::string> s_NodeToActionClassId;
    static std::unordered_map<int, std::unique_ptr<Params>> s_NodeToParams;
    static std::string s_SelectedActionClassName;

    template<typename ActionClass, typename ParamsStruct>
    static void AddActionNodeToBuilder(const std::string& nameAsID, const std::string& name = "")
    {
        ActionClassInfo actionInfo;
        actionInfo.NameAsID = nameAsID;
        actionInfo.Name = name;
        actionInfo.CreateParamsFn = []()
        {
            return std::make_unique<ParamsStruct>();
        };

        actionInfo.BuildFn = [](BehaviorTreeBuilder& builder, Node* node, Params& baseParams)
        {
            auto& params = static_cast<ParamsStruct&>(baseParams);
            builder.action<ActionClass>(node->Name, params);
        };
        s_ActionClassInfoMap.emplace(name, std::move(actionInfo));
    }
};
