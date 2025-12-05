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
BehaviorTree* NodeEditorApp::m_BehaviorTree = nullptr;
std::vector<HNode*> NodeEditorApp::m_ActiveNodes;
std::unordered_map<const HNode*, nodeEditor::NodeId> NodeEditorApp::s_NodeToEditorIdMap;
static bool s_InitLayout = true;
static float s_RightPanelWidth = 320.0f;


void NodeEditorApp::OnStart()
{
    NodeEditor::SpawnRootNode();
}

void NodeEditorApp::Update()
{
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::Button("Build", ImVec2(100, 30)))
        BuildBehaviorTree();
    if (ImGui::Button("Start", ImVec2(100, 30)))
    {
        if (m_BehaviorTree)
            m_BehaviorTree->StartTree();
    }

    auto lastHoveredNodeID = nodeEditor::GetHoveredNode();
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !io.KeyAlt)
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

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 workPos  = viewport->WorkPos;
    ImVec2 workSize = viewport->WorkSize;
    
    if (s_InitLayout)
    {
        ImGui::SetNextWindowPos(
            ImVec2(workPos.x + workSize.x - s_RightPanelWidth, workPos.y),
            ImGuiCond_Once
        );
        ImGui::SetNextWindowSize(
            ImVec2(s_RightPanelWidth, workSize.y),
            ImGuiCond_Once
        );
    }
    ImGui::Begin("Blackboard & Details", nullptr, ImGuiWindowFlags_NoMove | /*ImGuiWindowFlags_NoResize |*/ ImGuiWindowFlags_NoCollapse);

    ImGui::TextUnformatted("Blackboard / Node Details Panel");
    ImGui::Separator();
    ImGui::Text("Buraya blackboard ve node bilgileri gelecek.");

    ImGui::End();
    s_InitLayout = false;

    for (int i = 0; i + 1 < (int)m_ActiveNodes.size(); ++i)
    {
        HNode* activeNode = m_ActiveNodes[i];
        HNode* nextNode = m_ActiveNodes[i + 1];

        if (!activeNode || !nextNode)
            continue;
        if (nextNode->m_Parent != activeNode)
            continue;

        if (activeNode->GetStatus() != NodeStatus::RUNNING ||
            nextNode->GetStatus()   != NodeStatus::RUNNING)
            continue;
        
        Node* activeEditorNode = GetEditorNodeFor(activeNode);
        Node* nextEditorNode   = GetEditorNodeFor(nextNode);

        if (!activeEditorNode || !nextEditorNode)
            continue;

        if (activeEditorNode->Outputs.empty() || nextEditorNode->Inputs.empty())
            continue;

        auto outputPinID = activeEditorNode->Outputs[0].ID;
        auto inputPinID  = nextEditorNode->Inputs[0].ID;

        bool linkExists = false;
        auto linkID = nodeEditor::LinkId::Invalid;

        for (auto& link : NodeEditor::GetLinks())
        {
            if (link.StartPinID == outputPinID && link.EndPinID == inputPinID)
            {
                linkID = link.ID;
                linkExists = true;
                break;
            }
        }

        if (linkExists && linkID != nodeEditor::LinkId::Invalid)
            nodeEditor::Flow(linkID); //in imgui_node_editor in line 2964 i changed the duration for fade out
    }
}

void NodeEditorApp::ClearNodeMappings()
{
    s_NodeToEditorIdMap.clear();
}

void NodeEditorApp::RegisterNodeMapping(const HNode* runtimeNode, nodeEditor::NodeId editorId)
{
    if (!runtimeNode)
        return;

    s_NodeToEditorIdMap[runtimeNode] = editorId;
}

Node* NodeEditorApp::GetEditorNodeFor(const HNode* runtimeNode)
{
    if (!runtimeNode)
        return nullptr;

    auto it = s_NodeToEditorIdMap.find(runtimeNode);
    if (it == s_NodeToEditorIdMap.end())
        return nullptr;

    return NodeEditor::FindNode(it->second);
}

void NodeEditorApp::BuildBehaviorTree()
{
    NodeEditor::BuildNodes();
    std::cout << "Building Behavior Tree from Node Editor..." << std::endl;
    ClearNodeMappings();
    
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
                if (auto* runtimeNode = btBuilder.GetLastCreatedNode())
                    RegisterNodeMapping(runtimeNode, node->ID);
                break;
            case NodeType::Sequence:
                std::cout << "  Type: Sequence" << std::endl;
                if (!node->Decorators.empty())
                    for (auto& decorator : node->Decorators)
                        btBuilder.decorator<InverterDecorator>(decorator.Name);
            
                btBuilder.sequence(node->Name);
                if (auto* runtimeNode = btBuilder.GetLastCreatedNode())
                    RegisterNodeMapping(runtimeNode, node->ID);
            
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
                if (auto* runtimeNode = btBuilder.GetLastCreatedNode())
                    RegisterNodeMapping(runtimeNode, node->ID);
            
                if (!node->Conditions.empty())
                    for (auto& condition : node->Conditions)
                        btBuilder.condition<CanSeePlayerCondition>(PriortyType::Both, condition.Name, false);
                break;
            case NodeType::Action:
                std::cout << "  Type: Action" << std::endl;
                btBuilder.action<ActionNode>(node->Name, 10.0f);
                if (auto* runtimeNode = btBuilder.GetLastCreatedNode())
                    RegisterNodeMapping(runtimeNode, node->ID);
                break;
            case NodeType::end:
                std::cout << "  Type: End" << std::endl;
                btBuilder.end();
                break;
            default:
                break;
        }
    }
    btBuilder.end();
    m_BehaviorTree = btBuilder.build();
}
