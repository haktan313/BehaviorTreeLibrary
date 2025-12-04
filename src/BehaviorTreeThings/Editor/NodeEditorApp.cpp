#include "NodeEditorApp.h"
#include <iostream>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "CustomBlackboard.h"
#include "CustomNodes.h"
#include "imgui.h"
#include "NodeEditor.h"
#include "Tree.h"

EnemyAI* NodeEditorApp::m_Enemy = nullptr;

void NodeEditorApp::OnStart()
{
    NodeEditor::SpawnRootNode();
}

void NodeEditorApp::Update()
{
    if (ImGui::Button("Build", ImVec2(100, 30)))
        BuildBehaviorTree();
        
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        ImGui::OpenPopup("Node Context");
        
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
                btBuilder.sequence(node->Name);
                break;
            case NodeType::Selector:
                std::cout << "  Type: Selector" << std::endl;
                btBuilder.selector(node->Name);
                break;
            case NodeType::Action:
                std::cout << "  Type: Action" << std::endl;
                btBuilder.action<ActionNode>(node->Name, 10.0f);
                break;
            case NodeType::Condition:
                std::cout << "  Type: Condition" << std::endl;
                break;
            case NodeType::Decorator:
                std::cout << "  Type: Decorator" << std::endl;
                break;
            default:
                break;
        }
    }
    auto behaviorTree = btBuilder.build();
    behaviorTree->StartTree();
}
