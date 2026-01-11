#pragma once
#include <unordered_map>
#include "NodeEditorHelper.h"

class NodeEditorApp
{
public:
    void AddActiveNode(HNode* node) { m_ActiveNodes.push_back(node); }
    void RemoveActiveNode(HNode* node) { m_ActiveNodes.erase(std::remove(m_ActiveNodes.begin(), m_ActiveNodes.end(), node), m_ActiveNodes.end());}
    void ClearActiveNodes() { m_ActiveNodes.clear(); }
    //void SetEnemyAI(EnemyAI* enemy) { m_Enemy = enemy; }
    template<typename OwnerType>
    void SetOwner(OwnerType* owner)
    {
        m_Owner = static_cast<void*>(owner);
    }
    void ClearNodeMappings() { m_NodeToEditorIdMap.clear(); }
    
    NodeEditorApp();
    ~NodeEditorApp();
    
    void OnStart();
    void Update();

    void RegisterNodeMapping(const HNode* runtimeNode, nodeEditor::NodeId editorId);
    Node* GetEditorNodeFor(const HNode* runtimeNode);
    const HNode* GetRuntimeNodeFor(nodeEditor::NodeId editorId);
    std::unordered_map<const HNode*, nodeEditor::NodeId>& GetNodeMappings() { return m_NodeToEditorIdMap; }
    NodeEditorHelper& GetNodeEditorHelper() { return *m_NodeEditor; }
    HBlackboard& GetBlackboard() { return *m_Blackboard; }
    void* GetOwnerRaw() const { return m_Owner; }
    template<typename OwnerType>
    OwnerType* GetOwner() const
    {
        return static_cast<OwnerType*>(m_Owner);
    }
    
    void DecoratorNodeSelected(EditorDecorator& decorator);
    void ConditionNodeSelected(EditorCondition& condition);
    void DecoratorNodeUnSelected();
    void ConditionNodeUnSelected();
private:
    void* m_Owner = nullptr;
    
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
    
    void BuildBehaviorTree();
    void BuildSequence(Node* node, BehaviorTreeBuilder& btBuilder);
    void BuildSelector(Node* node, BehaviorTreeBuilder& btBuilder);
    void BuildAction(Node* node, BehaviorTreeBuilder& btBuilder);
    void ClearBuildData();
    void BuildPlanForNode(Node* editorNode, std::vector<BuildOp>& ops);
    std::vector<BuildOp> CreateBuildPlan();
    
    bool s_InitLayout = true;
    bool m_bDecoratorSelected = false;
    bool m_bConditionSelected = false;
    
    float s_RightPanelWidth = 320.0f;
    
    std::vector<HNode*> m_ActiveNodes;
    
    EditorDecorator* m_LastSelectedDecorator = nullptr;
    EditorCondition* m_LastSelectedCondition = nullptr;
    //EnemyAI* m_Enemy = nullptr;
    Node* m_LastHoveredNode = nullptr;
    Node* m_LastSelectedNode = nullptr;
    BehaviorTree* m_BehaviorTree = nullptr;
    std::unique_ptr<NodeEditorHelper> m_NodeEditor;
    std::unique_ptr<HBlackboard> m_Blackboard;
    
    std::unordered_map<const HNode*, nodeEditor::NodeId> m_NodeToEditorIdMap;
    std::unordered_map<uintptr_t, const HNode*> m_EditorIdToNodeMap;

    std::unordered_map<int, std::string> s_NodeToActionClassId;
    std::unordered_map<int, std::unique_ptr<ParamsForAction>> s_NodeToParams;
    std::string s_SelectedActionClassName;

    std::unordered_map<int, std::string> s_NodeToDecoratorClassId;
    std::unordered_map<int, std::unique_ptr<ParamsForDecorator>> s_NodeToDecoratorParams;
    std::string s_SelectedDecoratorClassName;

    std::unordered_map<int, std::string> s_NodeToConditionClassId;
    std::unordered_map<int, std::unique_ptr<ParamsForCondition>> s_NodeToConditionParams;
    std::string s_SelectedConditionClassName;

    std::string s_SelectedBlackboardClassName;

    std::string m_CurrentBTFilePath;

    friend class BTSerializer;
};
