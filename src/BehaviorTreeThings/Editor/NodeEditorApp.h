#pragma once
#include <unordered_map>
#include "NodeEditorHelper.h"

class NodeEditorApp
{
public:
    /*void AddActiveNode(HNode* node) { m_ActiveNodes.push_back(node); }
    void RemoveActiveNode(HNode* node) { m_ActiveNodes.erase(std::remove(m_ActiveNodes.begin(), m_ActiveNodes.end(), node), m_ActiveNodes.end());}
    void ClearActiveNodes() { m_ActiveNodes.clear(); }*/
    //void SetEnemyAI(EnemyAI* enemy) { m_Enemy = enemy; }
    void ClearNodeMappings() { m_NodeToEditorIdMap.clear(); }
    void ClearDatas();
    
    NodeEditorApp();
    ~NodeEditorApp();
    void SetEmbeddedMode(bool enabled) { m_bIsEmbedded = enabled; }
    void SetRuntimeMode(bool enabled) { m_bIsRuntimeMode = enabled; }
    void DrawToolbar();
    void DrawGraph();
    void DrawBlackboard();
    void DrawDebugBehaviorTree();
    
    void OnStart();
    void Update();

    void RegisterNodeMapping(const HNode* runtimeNode, nodeEditor::NodeId editorId);
    Node* GetEditorNodeFor(const HNode* runtimeNode);
    const HNode* GetRuntimeNodeFor(nodeEditor::NodeId editorId);
    std::unordered_map<const HNode*, nodeEditor::NodeId>& GetNodeMappings() { return m_NodeToEditorIdMap; }
    NodeEditorHelper& GetNodeEditorHelper() { return *m_NodeEditor; }
    HBlackboard& GetBlackboard() { return *m_Blackboard; }
    bool IsRuntimeMode() const { return m_bIsRuntimeMode; }

    void DecoratorNodeSelected(EditorDecorator& decorator);
    void ConditionNodeSelected(EditorCondition& condition);
    void DecoratorNodeUnSelected();
    void ConditionNodeUnSelected();
    
private:
    
    void MouseInputHandling();
    void NodeSettingsPanel();
    void FlowLinks();

    void BlackboardPanelSizeSettings();
    void BlackboardPanel();
    void ShowActionNodeInBlackboard();
    void ShowDecoratorNodeInBlackboard();
    void ShowConditionNodeInBlackboard();
    void ShowBlackboardDetails();
    HBlackboard& SetBlackboardForEditor(const std::string& id, const BlackboardClassInfo& info);

    BehaviorTree* BuildBehaviorTree();
    void BuildSequence(Node* node, BehaviorTreeBuilder& btBuilder);
    void BuildSelector(Node* node, BehaviorTreeBuilder& btBuilder);
    void BuildAction(Node* node, BehaviorTreeBuilder& btBuilder);
    void ClearBuildData();
    void BuildPlanForNode(Node* editorNode, std::vector<BuildOp>& ops);
    std::vector<BuildOp> CreateBuildPlan();
    
    void CreateEditorTreeFromRuntimeTree(BehaviorTree* runtimeTree);

    bool m_bDecoratorSelected = false;
    bool m_bConditionSelected = false;
    bool m_bIsEmbedded = false;
    bool m_bIsRuntimeMode = false;
    void DrawBlackboardContent();
    
    float s_RightPanelWidth = 320.0f;
    
    EditorDecorator* m_LastSelectedDecorator = nullptr;
    EditorCondition* m_LastSelectedCondition = nullptr;
    //EnemyAI* m_Enemy = nullptr;
    Node* m_LastHoveredNode = nullptr;
    Node* m_LastSelectedNode = nullptr;
    BehaviorTree* m_BehaviorTree = nullptr;
    std::unique_ptr<NodeEditorHelper> m_NodeEditor;
    std::unique_ptr<HBlackboard> m_Blackboard;
    HBlackboard* m_CopyBlackboard = nullptr;
    
    std::unordered_map<const HNode*, nodeEditor::NodeId> m_NodeToEditorIdMap;
    std::unordered_map<uintptr_t, const HNode*> m_EditorIdToNodeMap;

    std::unordered_map<int, std::string> m_NodeToActionClassId;
    std::unordered_map<int, std::unique_ptr<ParamsForAction>> m_NodeToParams;
    std::string m_SelectedActionClassName;

    std::unordered_map<int, std::string> m_NodeToDecoratorClassId;
    std::unordered_map<int, std::unique_ptr<ParamsForDecorator>> m_NodeToDecoratorParams;
    std::string m_SelectedDecoratorClassName;

    std::unordered_map<int, std::string> m_NodeToConditionClassId;
    std::unordered_map<int, std::unique_ptr<ParamsForCondition>> m_NodeToConditionParams;
    std::string m_SelectedConditionClassName;

    std::string m_SelectedBlackboardClassName;

    std::string m_CurrentBTFilePath;

    friend class BTSerializer;
};
