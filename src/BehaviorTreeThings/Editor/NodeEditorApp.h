#pragma once
#include <functional>
#include <unordered_map>
#include "NodeEditor.h"
#include "Tree.h"
#include "NodeEditorStructsAndEnums.h"

struct ActionClassInfo
{
    std::string Name;
    std::function<void(BehaviorTreeBuilder&, Node*, Params&)> BuildFn;
    std::function<std::unique_ptr<Params>()> CreateParamsFn;
};
struct DecoratorClassInfo
{
    std::string Name;
    std::function<void(BehaviorTreeBuilder&, Node*, Params&)> BuildFn;
    std::function<std::unique_ptr<Params>()> CreateParamsFn;
};
struct ConditionClassInfo
{
    std::string Name;
    std::function<void(BehaviorTreeBuilder&, Node*, ParamsForCondition&)> BuildFn;
    std::function<std::unique_ptr<ParamsForCondition>()> CreateParamsFn;
};
struct BlackboardClassInfo
{
    std::string Name;
    std::function<std::unique_ptr<HBlackboard>()> CreateBlackboardFn;
};

class NodeEditorApp
{
public:
    static void AddActiveNode(HNode* node) { m_ActiveNodes.push_back(node); }
    static void RemoveActiveNode() { if (!m_ActiveNodes.empty()) m_ActiveNodes.pop_back(); }
    static void ClearActiveNodes() { m_ActiveNodes.clear(); }
    
    NodeEditorApp();
    ~NodeEditorApp();
    
    void OnStart();
    void Update();
    void SetEnemyAI(EnemyAI* enemy) { m_Enemy = enemy; }

    void ClearNodeMappings();
    void RegisterNodeMapping(const HNode* runtimeNode, nodeEditor::NodeId editorId);
    Node* GetEditorNodeFor(const HNode* runtimeNode);
    
    void DecoratorNodeSelected(EditorDecorator& decorator);
    void ConditionNodeSelected(EditorCondition& condition);
    void DecoratorNodeUnSelected();
    void ConditionNodeUnSelected();
private:
    void MouseInputHandling();
    void NodeSettingsPanel();
    void FlowLinks();
    
    void BlackboardPanel();
    void BuildBehaviorTree();
    void BuildPlanForNode(Node* editorNode, std::vector<BuildOp>& ops);
    std::vector<BuildOp> CreateBuildPlan();

    static std::vector<HNode*> m_ActiveNodes;

    bool s_InitLayout = true;
    bool m_bDecoratorSelected = false;
    bool m_bConditionSelected = false;
    
    float s_RightPanelWidth = 320.0f;
    
    EditorDecorator* m_LastSelectedDecorator = nullptr;
    EditorCondition* m_LastSelectedCondition = nullptr;
    EnemyAI* m_Enemy = nullptr;
    Node* m_LastHoveredNode = nullptr;
    Node* m_LastSelectedNode = nullptr;
    BehaviorTree* m_BehaviorTree = nullptr;
    std::unique_ptr<NodeEditor> m_NodeEditor;
    std::unique_ptr<HBlackboard> m_Blackboard;
    
    std::unordered_map<const HNode*, nodeEditor::NodeId> s_NodeToEditorIdMap;
    
    std::unordered_map<std::string, ActionClassInfo> s_ActionClassInfoMap;
    std::unordered_map<int, std::string> s_NodeToActionClassId;
    std::unordered_map<int, std::unique_ptr<Params>> s_NodeToParams;
    std::string s_SelectedActionClassName;

    std::unordered_map<std::string, DecoratorClassInfo> s_DecoratorClassInfoMap;
    std::unordered_map<int, std::string> s_NodeToDecoratorClassId;
    std::unordered_map<int, std::unique_ptr<Params>> s_NodeToDecoratorParams;
    std::string s_SelectedDecoratorClassName;

    std::unordered_map<std::string, ConditionClassInfo> s_ConditionClassInfoMap;
    std::unordered_map<int, std::string> s_NodeToConditionClassId;
    std::unordered_map<int, std::unique_ptr<ParamsForCondition>> s_NodeToConditionParams;
    std::string s_SelectedConditionClassName;

    std::unordered_map<std::string, BlackboardClassInfo> s_BlackboardClassInfoMap;
    std::string s_SelectedBlackboardClassName;

    template<typename ActionClass, typename ParamsStruct>
    void AddActionNodeToBuilder(const std::string& nameAsID, const std::string& name = "")
    {
        ActionClassInfo actionInfo;
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
    template<typename DecoratorClass, typename ParamsStruct>
    void AddDecoratorNodeToBuilder(const std::string& nameAsID, const std::string& name = "")
    {
        DecoratorClassInfo decoratorInfo;
        decoratorInfo.Name = name;
        decoratorInfo.CreateParamsFn = []()
        {
            return std::make_unique<ParamsStruct>();
        };
        decoratorInfo.BuildFn = [](BehaviorTreeBuilder& builder, Node* node, Params& baseParams)
        {
            auto& params = static_cast<ParamsStruct&>(baseParams);
            builder.decorator<DecoratorClass>(node->Name, params);
        };
        s_DecoratorClassInfoMap.emplace(name, std::move(decoratorInfo));
    }
    template<typename ConditionClass, typename ParamsStruct>
    void AddConditionNodeToBuilder(const std::string& nameAsID, const std::string& name = "")
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
            builder.condition<ConditionClass>(baseParams.Priorty, node->Name, params);
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
