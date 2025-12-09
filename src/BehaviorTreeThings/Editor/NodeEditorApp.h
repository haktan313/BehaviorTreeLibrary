#pragma once
#include <unordered_map>
#include "NodeEditorHelper.h"
#include "Tree.h"
#include "NodeEditorStructsAndEnums.h"

class NodeEditorApp
{
public:
    void AddActiveNode(HNode* node) { m_ActiveNodes.push_back(node); }
    void RemoveActiveNode(HNode* node) { m_ActiveNodes.erase(std::remove(m_ActiveNodes.begin(), m_ActiveNodes.end(), node), m_ActiveNodes.end());}
    void ClearActiveNodes() { m_ActiveNodes.clear(); }
    void SetEnemyAI(EnemyAI* enemy) { m_Enemy = enemy; }
    void ClearNodeMappings() { s_NodeToEditorIdMap.clear(); }
    
    NodeEditorApp();
    ~NodeEditorApp();
    
    void OnStart();
    void Update();

    void RegisterNodeMapping(const HNode* runtimeNode, nodeEditor::NodeId editorId);
    Node* GetEditorNodeFor(const HNode* runtimeNode);
    
    void DecoratorNodeSelected(EditorDecorator& decorator);
    void ConditionNodeSelected(EditorCondition& condition);
    void DecoratorNodeUnSelected();
    void ConditionNodeUnSelected();
    bool CheckConditionsSelfMode(HNode* node, const std::vector<std::unique_ptr<HCondition>>& conditionNodes);
    void CheckConditionsLowerPriorityMode(int& currentChildIndex, HNode* node, const std::vector<std::unique_ptr<HNode>>& childrens);

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
    EnemyAI* m_Enemy = nullptr;
    Node* m_LastHoveredNode = nullptr;
    Node* m_LastSelectedNode = nullptr;
    BehaviorTree* m_BehaviorTree = nullptr;
    std::unique_ptr<NodeEditorHelper> m_NodeEditor;
    std::unique_ptr<HBlackboard> m_Blackboard;
    
    std::unordered_map<const HNode*, nodeEditor::NodeId> s_NodeToEditorIdMap;
    
    std::unordered_map<std::string, ActionClassInfo> s_ActionClassInfoMap;
    std::unordered_map<int, std::string> s_NodeToActionClassId;
    std::unordered_map<int, std::unique_ptr<ParamsForAction>> s_NodeToParams;
    std::string s_SelectedActionClassName;

    std::unordered_map<std::string, DecoratorClassInfo> s_DecoratorClassInfoMap;
    std::unordered_map<int, std::string> s_NodeToDecoratorClassId;
    std::unordered_map<int, std::unique_ptr<ParamsForDecorator>> s_NodeToDecoratorParams;
    std::string s_SelectedDecoratorClassName;

    std::unordered_map<std::string, ConditionClassInfo> s_ConditionClassInfoMap;
    std::unordered_map<int, std::string> s_NodeToConditionClassId;
    std::unordered_map<int, std::unique_ptr<ParamsForCondition>> s_NodeToConditionParams;
    std::string s_SelectedConditionClassName;

    std::unordered_map<std::string, BlackboardClassInfo> s_BlackboardClassInfoMap;
    std::string s_SelectedBlackboardClassName;

    template<typename ActionClass, typename ParamsStruct>
    void AddActionNodeToBuilder(const std::string& name = "")
    {
        ActionClassInfo actionInfo;
        actionInfo.Name = name;
        actionInfo.CreateParamsFn = []()
        {
            return std::make_unique<ParamsStruct>();
        };

        actionInfo.BuildFn = [](BehaviorTreeBuilder& builder, Node* node, ParamsForAction& baseParams)
        {
            auto& params = static_cast<ParamsStruct&>(baseParams);
            builder.action<ActionClass>(node->Name, params);
        };
        s_ActionClassInfoMap.emplace(name, std::move(actionInfo));
    }
    template<typename DecoratorClass, typename ParamsStruct>
    void AddDecoratorNodeToBuilder(const std::string& name = "")
    {
        DecoratorClassInfo decoratorInfo;
        decoratorInfo.Name = name;
        decoratorInfo.CreateParamsFn = []()
        {
            return std::make_unique<ParamsStruct>();
        };
        decoratorInfo.BuildFn = [](BehaviorTreeBuilder& builder, Node* node, ParamsForDecorator& baseParams)
        {
            auto& params = static_cast<ParamsStruct&>(baseParams);
            builder.decorator<DecoratorClass>(node->Name, params);
        };
        s_DecoratorClassInfoMap.emplace(name, std::move(decoratorInfo));
    }
    template<typename ConditionClass, typename ParamsStruct>
    void AddConditionNodeToBuilder(const std::string& name = "")
    {
        ConditionClassInfo conditionInfo;
        conditionInfo.Name = name;
        conditionInfo.CreateParamsFn = []()
        {
            return std::make_unique<ParamsStruct>();
        };
        conditionInfo.BuildFn = [](BehaviorTreeBuilder& builder, Node* node, ParamsForCondition& baseParams)
        {
            auto& params = static_cast<ParamsStruct&>(baseParams);
            builder.condition<ConditionClass>(baseParams.Priority, node->Name, params);
        };
        s_ConditionClassInfoMap.emplace(name, std::move(conditionInfo));
    }
    template<typename BlackboardType>
    void AddBlackBoardToEditor(const std::string& name = "")
    {
        BlackboardClassInfo blackboardInfo;
        blackboardInfo.Name = name;
        blackboardInfo.CreateBlackboardFn = []()
        {
            return std::make_unique<BlackboardType>();
        };
        s_BlackboardClassInfoMap.emplace(name, std::move(blackboardInfo));
    }   
};
