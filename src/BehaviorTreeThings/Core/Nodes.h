#pragma once
#include <memory>
#include <string>
#include <vector>

#include "BlackboardBase.h"
#include "EnumsForTree.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

class HCondition;
class HBlackboard;
class EnemyAI;
class NodeEditorApp;

class HNode
{
public:
    HNode(const std::string& name) : m_Name(name), m_Parent(nullptr), m_Status(NodeStatus::FAILURE), m_EditorApp(nullptr) {}
    virtual ~HNode() = default;
    
    NodeStatus Tick();
    virtual void OnStart();
    virtual NodeStatus Update() = 0;
    virtual void OnFinished();
    virtual void OnAbort();
    
    virtual void AddChild(std::unique_ptr<HNode> child);
    virtual void AddConditionNode(std::unique_ptr<HCondition> conditionNode);

    virtual bool CanStart() { return true; }
    
    void SetParent(HNode* parent) { m_Parent = parent; }
    void SetEditorApp(NodeEditorApp* app) { m_EditorApp = app; }
    void SetType(HNodeType type) { m_Type = type; }
    
    HNode* GetParent() const { return m_Parent; }
    NodeEditorApp* GetEditorApp() { return m_EditorApp; }
    NodeStatus GetStatus() const { return m_Status; }
    HNodeType GetType() const { return m_Type; }
    std::string GetName() const { return m_Name; }
    std::vector<std::unique_ptr<HNode>>& GetChildrens() { return m_Childrens; }
    std::vector<std::unique_ptr<HCondition>>& GetConditionNodes() { return m_ConditionNodes; }
    bool m_bIsStarted = false;
protected:
    HNode* m_Parent;
    NodeEditorApp* m_EditorApp;
    std::string m_Name;
    NodeStatus m_Status;
    HNodeType m_Type;
    std::vector<std::unique_ptr<HNode>> m_Childrens;
    std::vector<std::unique_ptr<HCondition>> m_ConditionNodes;
};

class HRootNode : public HNode
{
public:
    HRootNode() : HNode("Root") {}

    virtual void OnStart() override;
    virtual NodeStatus Update() override;
    virtual void OnFinished() override;
    virtual void OnAbort() override;

    void AddChild(std::unique_ptr<HNode> child) override;
    void AddConditionNode(std::unique_ptr<HCondition> conditionNode) override {}
};

struct Params
{
    Params() = default;
    ~Params() = default;
    
    virtual void DrawImGui(HBlackboard* blackboard) {}
    void DrawFloatValue(const std::string& label, float& value)
    {
        ImGui::InputFloat(label.c_str(), &value);
    }
    void DrawIntValue(const std::string& label, int& value)
    {
        ImGui::InputInt(label.c_str(), &value);
    }
    void DrawBoolValue(const std::string& label, bool& value)
    {
        ImGui::Checkbox(label.c_str(), &value);
    }
    void DrawStringValue(const std::string& label, std::string& value)
    {
        char buffer[256];
        std::strncpy(buffer, value.c_str(), sizeof(buffer));
        if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer)))
        {
            value = std::string(buffer);
        }
    }
    void DrawBlackboardFloatKeySelector(const std::string& label, std::string& key, HBlackboard* blackboard)
    {
        const char* preview = key.empty() ? "Select float key..." : key.c_str();
        if (ImGui::BeginCombo(label.c_str(), preview))
        {
            if (blackboard)
                for (const auto& [bbKey, value] : blackboard->GetFloatValues())
                {
                    bool isSelected = (key == bbKey);
                    if (ImGui::Selectable(bbKey.c_str(), isSelected))
                        key = bbKey;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
            ImGui::EndCombo();
        }
    }
    void DrawBlackboardIntKeySelector(const std::string& label, std::string& key, HBlackboard* blackboard)
    {
        const char* preview = key.empty() ? "Select int key..." : key.c_str();
        if (ImGui::BeginCombo(label.c_str(), preview))
        {
            if (blackboard)
                for (const auto& [bbKey, value] : blackboard->GetIntValues())
                {
                    bool isSelected = (key == bbKey);
                    if (ImGui::Selectable(bbKey.c_str(), isSelected))
                        key = bbKey;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
            ImGui::EndCombo();
        }
    }
    void DrawBlackboardBoolKeySelector(const std::string& label, std::string& key, HBlackboard* blackboard)
    {
        const char* preview = key.empty() ? "Select bool key..." : key.c_str();
        if (ImGui::BeginCombo(label.c_str(), preview))
        {
            if (blackboard)
                for (const auto& [bbKey, value] : blackboard->GetBoolValues())
                {
                    bool isSelected = (key == bbKey);
                    if (ImGui::Selectable(bbKey.c_str(), isSelected))
                        key = bbKey;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
            ImGui::EndCombo();
        }
    }
    void DrawBlackboardStringKeySelector(const std::string& label, std::string& key, HBlackboard* blackboard)
    {
        const char* preview = key.empty() ? "Select string key..." : key.c_str();
        if (ImGui::BeginCombo(label.c_str(), preview))
        {
            if (blackboard)
                for (const auto& [bbKey, value] : blackboard->GetStringValues())
                {
                    bool isSelected = (key == bbKey);
                    if (ImGui::Selectable(bbKey.c_str(), isSelected))
                        key = bbKey;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
            ImGui::EndCombo();
        }
    }
};
class HActionNode : public HNode
{
public:
    HActionNode(const std::string& name, const Params& params = Params{}) : HNode(name), m_Owner(nullptr), m_Blackboard(nullptr) {}
    
    virtual void OnStart() override;
    virtual NodeStatus Update() override;
    virtual void OnFinished() override;
    virtual void OnAbort() override;

    bool CanStart() override;
    
    bool CheckConditions();
    bool CheckConditionsSelfMode();
protected:
    EnemyAI& GetOwner() const { return *m_Owner; }
    HBlackboard& GetBlackboard() const { return *m_Blackboard; }
private:
    EnemyAI* m_Owner;
    HBlackboard* m_Blackboard;
    
    void SetOwner(EnemyAI* owner) { m_Owner = owner; }
    void SetBlackboard(HBlackboard* blackboard) { m_Blackboard = blackboard; }
    friend class BehaviorTreeBuilder;
    friend class BehaviorTree;
};

struct ParamsForCondition : public Params
{
    ParamsForCondition() = default;
    ~ParamsForCondition() = default;

    virtual void DrawImGui(HBlackboard* blackboard) override {}

    PriorityType Priority = PriorityType::None;
};
class HCondition : public HNode
{
public:
    HCondition(const std::string& name, const ParamsForCondition& params = ParamsForCondition{})
        : HNode(name), m_Owner(nullptr), m_Blackboard(nullptr), m_PriorityMode(PriorityType::None), m_LastStatus(NodeStatus::RUNNING) {}

    virtual void OnStart() override {}
    virtual bool CheckCondition() = 0;
    virtual void OnFinished() override { m_bIsStarted = false; }
    virtual void OnAbort() override { HNode::OnAbort(); }

    void SetLastStatus(NodeStatus status) { m_LastStatus = status; }

    PriorityType GetPriorityMode() const { return m_PriorityMode; }
    NodeStatus GetLastStatus() const { return m_LastStatus; }
protected:
    EnemyAI& GetOwner() const { return *m_Owner; }
    HBlackboard& GetBlackboard() const { return *m_Blackboard; }
private:
    EnemyAI* m_Owner;
    HBlackboard* m_Blackboard;
    PriorityType m_PriorityMode;
    NodeStatus m_LastStatus;

    virtual NodeStatus Update() override final { return CheckCondition() ? NodeStatus::SUCCESS : NodeStatus::FAILURE; }
    void SetOwner(EnemyAI* owner) { m_Owner = owner; }
    void SetBlackboard(HBlackboard* blackboard) { m_Blackboard = blackboard; }
    void SetPriorityMode(PriorityType priority) { m_PriorityMode = priority; }
    friend class BehaviorTreeBuilder;
    friend class BehaviorTree;
};

struct ParamsForDecorator : public Params
{
    ParamsForDecorator() = default;
    ~ParamsForDecorator() = default;

    virtual void DrawImGui(HBlackboard* blackboard) override {}
}; 
class HDecorator : public HNode
{
public:
    HDecorator(const std::string& name, const ParamsForDecorator& params = ParamsForDecorator{}) : HNode(name), m_Blackboard(nullptr), m_Owner(nullptr) {}

    virtual void OnStart() override {}
    virtual bool CanExecute() = 0;
    virtual void OnFinishedResult(NodeStatus& status) = 0;
    virtual void OnFinished() override { m_bIsStarted = false; }
    virtual void OnAbort() override { HNode::OnAbort(); }
protected:
    EnemyAI& GetOwner() const { return *m_Owner; }
    HBlackboard& GetBlackboard() const { return *m_Blackboard; }
private:
    EnemyAI* m_Owner;
    HBlackboard* m_Blackboard;

    virtual NodeStatus Update() override final;
    void SetOwner(EnemyAI* owner) { m_Owner = owner; }
    void SetBlackboard(HBlackboard* blackboard) { m_Blackboard = blackboard; }
    friend class BehaviorTreeBuilder;
    friend class BehaviorTree;
};