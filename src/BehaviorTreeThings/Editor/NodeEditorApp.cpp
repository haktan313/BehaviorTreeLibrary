#include "NodeEditorApp.h"
#include <iostream>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "CustomBlackboard.h"
#include "CustomNodes.h"
#include "imgui.h"
#include "NodeEditor.h"

EnemyAI* NodeEditorApp::m_Enemy = nullptr;
Node* NodeEditorApp::m_LastHoveredNode = nullptr;
Node* NodeEditorApp::m_LastSelectedNode = nullptr;
BehaviorTree* NodeEditorApp::m_BehaviorTree = nullptr;
std::vector<HNode*> NodeEditorApp::m_ActiveNodes;
std::unordered_map<const HNode*, nodeEditor::NodeId> NodeEditorApp::s_NodeToEditorIdMap;
static bool s_InitLayout = true;
static float s_RightPanelWidth = 320.0f;

std::unordered_map<std::string, ActionClassInfo> NodeEditorApp::s_ActionClassInfoMap;
std::string NodeEditorApp::s_SelectedActionClassName = "";
std::unordered_map<int, std::string> NodeEditorApp::s_NodeToActionClassId;
std::unordered_map<int, std::unique_ptr<Params>> NodeEditorApp::s_NodeToParams;

void NodeEditorApp::OnStart()
{
    NodeEditor::SpawnRootNode();
    AddActionNodeToBuilder<MoveToNode, MoveToNodeParams>("MoveToNode", "Move To");
    AddActionNodeToBuilder<ActionNode, ActionNodeParams>("ActionNode", "Generic Action");
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
    m_LastSelectedNode = NodeEditor::GetSelectedNode();
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

    BlackboardPanel();

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

void NodeEditorApp::BlackboardPanel()
{
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
    ImGui::Begin("Blackboard & Details", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::TextUnformatted("Blackboard / Node Details Panel");
    ImGui::Separator();

    /*if (m_LastSelectedNode && m_LastSelectedNode->Type == NodeType::Action)
    {
        ImGui::Text("Action Class");
        ImGui::Separator();

        if (ImGui::BeginCombo("##ActionList", s_SelectedActionClassName.c_str()))
        {
            for (auto& [key, info] : s_ActionClassInfoMap)
                if(ImGui::Selectable(info.Name.c_str()))
                    s_SelectedActionClassName = info.Name.c_str();
            ImGui::EndCombo();
        }
        ImGui::Text("Parameters");
        ImGui::Separator();
        if (!s_SelectedActionClassName.empty())
            s_ActionClassInfoMap[s_SelectedActionClassName].Params->DrawImGui();
    }*/
    if (m_LastSelectedNode && m_LastSelectedNode->Type == NodeType::Action)
    {
        int nodeKey = (int)m_LastSelectedNode->ID.Get();

        ImGui::Text("Action Class");
        ImGui::Separator();

        std::string currentIdString;
        auto it = s_NodeToActionClassId.find(nodeKey);
        if (it != s_NodeToActionClassId.end())
            currentIdString = it->second;

        std::string currentLabel = "Select Action";
        if (!currentIdString.empty())
        {
            auto info = s_ActionClassInfoMap.find(currentIdString);
            if (info != s_ActionClassInfoMap.end())
                currentLabel = info->second.Name;
        }

        if (ImGui::BeginCombo("##ActionList", currentLabel.c_str()))
        {
            for (auto& [id, info] : s_ActionClassInfoMap)
            {
                bool isSelected = (id == currentIdString);
                if (ImGui::Selectable(info.Name.c_str(), isSelected))
                {
                    s_NodeToActionClassId[nodeKey] = id;
                    s_NodeToParams[nodeKey] = info.CreateParamsFn();
                    if (m_LastSelectedNode)
                        m_LastSelectedNode->Name = info.Name;
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Parameters");
        ImGui::Separator();

        if (!currentIdString.empty())
        {
            auto parameter = s_NodeToParams.find(nodeKey);
            if (parameter != s_NodeToParams.end() && parameter->second)
                parameter->second->DrawImGui();
        }
    }

    ImGui::End();
    s_InitLayout = false;
}


void NodeEditorApp::BuildBehaviorTree()
{
    NodeEditor::BuildNodes();
    std::cout << "Building Behavior Tree from Node Editor..." << std::endl;
    ClearNodeMappings();
    
    BehaviorTreeBuilder btBuilder(m_Enemy);
    btBuilder.setBlackboard<EnemyBlackboard>();
    
    btBuilder.root();
    if (auto* runtimeRoot = btBuilder.GetLastCreatedNode())
        for (auto& n : NodeEditor::GetNodes())
        {
            if (n.Type == NodeType::Root)
            {
                RegisterNodeMapping(runtimeRoot, n.ID);
                break;
            }
        }
    
    auto buildOps = CreateBuildPlan();
    for (BuildOp& op : buildOps)
        switch (op.Type)
        {
            case BuildOpType::OpenComposite:
            {
                Node* node = op.EditorNode;
                if (node->Type == NodeType::Sequence)
                {
                    for (auto& decorator : node->Decorators)
                        btBuilder.decorator<InverterDecorator>(decorator.Name);
                    btBuilder.sequence(node->Name);
                }
                else if (node->Type == NodeType::Selector)
                {
                    for (auto& decorator : node->Decorators)
                        btBuilder.decorator<InverterDecorator>(decorator.Name);
                    btBuilder.selector(node->Name);
                }
                    
                for (auto& condition : node->Conditions)
                    btBuilder.condition<CanSeePlayerCondition>(PriortyType::Both, condition.Name, false);
                    
                if (auto* runtimeNode = btBuilder.GetLastCreatedNode())
                    RegisterNodeMapping(runtimeNode, node->ID);
                break;
            }
            case BuildOpType::Action:
            {
                Node* node = op.EditorNode;
                int nodeKey = (int)node->ID.Get();
                auto classIt = s_NodeToActionClassId.find(nodeKey);
                if (classIt == s_NodeToActionClassId.end())
                    break;
                    
                const std::string& classId = classIt->second;
                auto infoIt = s_ActionClassInfoMap.find(classId);
                if (infoIt == s_ActionClassInfoMap.end())
                    break;
                    
                ActionClassInfo& info = infoIt->second;
                auto paramsIt = s_NodeToParams.find(nodeKey);
                if (paramsIt == s_NodeToParams.end() || !paramsIt->second)
                    break;
                    
                Params& params = *paramsIt->second;
                info.BuildFn(btBuilder, node, params);              
                if (auto* runtimeNode = btBuilder.GetLastCreatedNode())
                    RegisterNodeMapping(runtimeNode, node->ID);
                break;
            }
            case BuildOpType::CloseComposite:
            {
                btBuilder.end();
                break;
            }
            default:
                break;
        }
    m_BehaviorTree = btBuilder.build();
}

void NodeEditorApp::BuildPlanForNode(Node* editorNode, std::vector<BuildOp>& ops)
{
    switch (editorNode->Type)
    {
    case NodeType::Root:
        {
            auto children = NodeEditor::GetChilderenNodes(editorNode);
            if (!children.empty())
                BuildPlanForNode(children[0], ops);
            break;
        }
    case NodeType::Sequence:
    case NodeType::Selector:
        {
            ops.push_back({ BuildOpType::OpenComposite, editorNode });
            
            auto children = NodeEditor::GetChilderenNodes(editorNode);
            std::sort(children.begin(), children.end(),
                [](Node* a, Node* b)
                {
                    ImVec2 pa = nodeEditor::GetNodePosition(a->ID);
                    ImVec2 pb = nodeEditor::GetNodePosition(b->ID);
                    if (pa.x == pb.x)
                        return pa.y < pb.y;
                    return pa.x < pb.x;
                });         
            for (Node* child : children)
                BuildPlanForNode(child, ops);
            
            ops.push_back({ BuildOpType::CloseComposite, nullptr });
            break;
        }
    case NodeType::Action:
        {
            ops.push_back({ BuildOpType::Action, editorNode });
            break;
        }
    default:
        break;
    }
}

std::vector<BuildOp> NodeEditorApp::CreateBuildPlan()
{
    std::vector<BuildOp> ops;
    
    Node* rootEditorNode = nullptr;
    for (auto& n : NodeEditor::GetNodes())
        if (n.Type == NodeType::Root)
        {
            rootEditorNode = &n;
            break;
        }

    if (!rootEditorNode)
    {
        std::cerr << "Root editor node not found!\n";
        return ops;
    }

    BuildPlanForNode(rootEditorNode, ops);
    return ops;
}
