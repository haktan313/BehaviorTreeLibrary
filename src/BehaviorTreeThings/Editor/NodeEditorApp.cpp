#include "NodeEditorApp.h"
#include <iostream>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "NodeEditor.h"

NodeEditorApp* NodeEditorApp::s_Instance = nullptr;

NodeEditorApp::NodeEditorApp()
{
    s_Instance = this;
}

NodeEditorApp::~NodeEditorApp()
{
    s_Instance = nullptr;
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
    auto links = NodeEditor::GetLinks();
    auto nodes = NodeEditor::GetNodes();
    for (auto& node : nodes)
    {
        std::cout << "Node: " << node.Name << " (ID: " << node.ID.Get() << ")" << std::endl;
    }
    for (auto& link : links)
    {
        std::cout << "Link: " << link.ID.Get() << " from Pin "<< link.StartPinID.Get() << " to Pin " << link.EndPinID.Get() << std::endl;
        auto startPin = NodeEditor::FindPin(link.StartPinID);
        auto endPin = NodeEditor::FindPin(link.EndPinID);
        if (startPin && endPin)
        {
            std::cout << "  Start Node: " << startPin->Node->Name << " (ID: " << startPin->Node->ID.Get() << ")" << std::endl;
            std::cout << "  End Node: " << endPin->Node->Name << " (ID: " << endPin->Node->ID.Get() << ")" << std::endl;
        }
    } 
}
