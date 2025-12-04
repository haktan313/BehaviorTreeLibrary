#include "NodeEditorApp.h"
#include <iostream>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "CustomBlackboard.h"
#include "CustomNodes.h"
#include "imgui.h"
#include "NodeEditor.h"
#include "Tree.h"

EnemyAI* NodeEditorApp::m_Enemy = nullptr;
Node* NodeEditorApp::m_LastHoveredNode = nullptr;

void NodeEditorApp::OnStart()
{
    NodeEditor::SpawnRootNode();
}

void NodeEditorApp::Update()
{
    if (ImGui::Button("Build", ImVec2(100, 30)))
        BuildBehaviorTree();

    auto lastHoveredNodeID = nodeEditor::GetHoveredNode();
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        bool isNodeHovered = nodeEditor::GetHoveredNode() || nodeEditor::GetHoveredPin() || nodeEditor::GetHoveredLink();
        if (!isNodeHovered)
        {
            m_LastHoveredNode = nullptr;
            ImGui::OpenPopup("Node Context");
        }
        else
        {
            m_LastHoveredNode = NodeEditor::FindNode(lastHoveredNodeID);
            if (m_LastHoveredNode->Type == NodeType::Sequence || m_LastHoveredNode->Type == NodeType::Selector)
                ImGui::OpenPopup("Node Options");
        }
    }
        
    if (ImGui::BeginPopup("Node Context"))
    {
        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();

        ImVec2 mouseScreenPos = ImGui::GetMousePosOnOpeningCurrentPopup();
        ImVec2 mouseCanvasPos = nodeEditor::ScreenToCanvas(mouseScreenPos);
        if (ImGui::MenuItem("Create Sequence"))
            NodeEditor::SpawnSequenceNode(mouseCanvasPos);
        if (ImGui::MenuItem("Create Selector"))
            NodeEditor::SpawnSelectorNode(mouseCanvasPos);
        if (ImGui::MenuItem("Create Action"))
            NodeEditor::SpawnActionNode(mouseCanvasPos);
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("Node Options"))
    {
        ImGui::TextUnformatted("Node Options Menu");
        ImGui::Separator();
        if (ImGui::MenuItem("Create Decorator"))
            NodeEditor::SpawnDecoratorNode(m_LastHoveredNode);
        if (ImGui::MenuItem("Create Condition"))
            NodeEditor::SpawnConditionNode(m_LastHoveredNode);
        ImGui::EndPopup();
    }
    int linkAmount = (int)NodeEditor::GetLinks().size();
    ImGui::Text("Links: %d", linkAmount);
}

void NodeEditorApp::BuildBehaviorTree()
{
    NodeEditor::BuildNodes();
    std::cout << "Building Behavior Tree from Node Editor..." << std::endl;
    auto btBuilder = BehaviorTreeBuilder(m_Enemy);
    btBuilder.setBlackboard<EnemyBlackboard>();
    auto nodes = NodeEditor::GetNodesBuildOrder();
    for (auto& node : nodes)
    {
        std::cout << "Node: " << node->Name << " (ID: " << node->ID.Get() << ")" << std::endl;
        switch (node->Type)
        {
            case NodeType::Root:
                std::cout << "  Type: Root" << std::endl;
                btBuilder.root();
                break;
            case NodeType::Sequence:
                std::cout << "  Type: Sequence" << std::endl;
                if (!node->Decorators.empty())
                    for (auto& decorator : node->Decorators)
                        btBuilder.decorator<InverterDecorator>(decorator.Name);
            
                btBuilder.sequence(node->Name);
            
                if (!node->Conditions.empty())
                    for (auto& condition : node->Conditions)
                        btBuilder.condition<CanSeePlayerCondition>(PriortyType::Both, condition.Name, false);
                break;
            case NodeType::Selector:
                std::cout << "  Type: Selector" << std::endl;
                if (!node->Decorators.empty())
                    for (auto& decorator : node->Decorators)
                        btBuilder.decorator<InverterDecorator>(decorator.Name);
            
                btBuilder.selector(node->Name);
            
                if (!node->Conditions.empty())
                    for (auto& condition : node->Conditions)
                        btBuilder.condition<CanSeePlayerCondition>(PriortyType::Both, condition.Name, false);
                break;
            case NodeType::Action:
                std::cout << "  Type: Action" << std::endl;
                btBuilder.action<ActionNode>(node->Name, 10.0f);
                break;
            default:
                break;
        }
    }
    auto behaviorTree = btBuilder.build();
    behaviorTree->StartTree();
}
