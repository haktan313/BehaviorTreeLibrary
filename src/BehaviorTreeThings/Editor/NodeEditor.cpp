

#include "NodeEditor.h"
#include <iostream>

namespace nodeEditor = ax::NodeEditor;

NodeEditor* NodeEditor::s_Instance = nullptr;
std::vector<Node> NodeEditor::m_Nodes;
std::vector<Link> NodeEditor::m_Links;
std::map<nodeEditor::NodeId, float, NodeIdLess> NodeEditor::m_NodeTouchTime;
nodeEditor::PinId NodeEditor::m_RootOutputPinId = 0;

static Pin* newLinkPin = nullptr;
static Pin* newNodeLinkPin = nullptr;
static bool createNewNode = false;
static const float m_TouchTime = 1.0f;
static nodeEditor::EditorContext* m_EditorContext = nullptr;
static int m_NextId = 1;

NodeEditor::NodeEditor()
{
    s_Instance = this;
}

Node* NodeEditor::FindNode(nodeEditor::NodeId id)
{
    for (auto& node : m_Nodes)
        if (node.ID == id)
            return &node;

    return nullptr;
}
void NodeEditor::TouchNode(nodeEditor::NodeId id)
{
    m_NodeTouchTime[id] = m_TouchTime;
}
void NodeEditor::BuildNodes()
{
    for (auto& node : m_Nodes)
        BuildNode(&node);
}

bool NodeEditor::CanCreateLink(Pin* a, Pin* b)
{
    if (!a || !b || a == b || a->Kind == b->Kind || a->Node == b->Node)
        return false;

    return true;
}

Pin* NodeEditor::FindPin(nodeEditor::PinId id)
{
    if (!id)
        return nullptr;

    for (auto& node : m_Nodes)
    {
        for (auto& pin : node.Inputs)
            if (pin.ID == id)
                return &pin;

        for (auto& pin : node.Outputs)
            if (pin.ID == id)
                return &pin;
    }

    return nullptr;
}

int NodeEditor::GetNextID()
{
    return m_NextId++;
}

/*ImColor GetIconColor(PinType type)
{
    switch (type)
    {
    default:
    case PinType::Flow:     return ImColor(255, 255, 255);
    case PinType::Bool:     return ImColor(220,  48,  48);
    case PinType::Int:      return ImColor( 68, 201, 156);
    case PinType::Float:    return ImColor(147, 226,  74);
    case PinType::String:   return ImColor(124,  21, 153);
    case PinType::Object:   return ImColor( 51, 150, 215);
    case PinType::Function: return ImColor(218,   0, 183);
    case PinType::Delegate: return ImColor(255,  48,  48);
    }
};*/

void NodeEditor::OnStart()
{
    nodeEditor::Config config;
    config.UserPointer = s_Instance;

    config.LoadNodeSettings = [](nodeEditor::NodeId nodeId, char* data, void* userPointer) -> size_t
    {
        auto self = static_cast<NodeEditor*>(userPointer);

        auto node = self->FindNode(nodeId);
        if (!node)
            return 0;

        if (data != nullptr)
            memcpy(data, node->State.data(), node->State.size());
        return node->State.size();
    };

    config.SaveNodeSettings = [](nodeEditor::NodeId nodeId, const char* data, size_t size, nodeEditor::SaveReasonFlags reason, void* userPointer) -> bool
    {
        auto self = static_cast<NodeEditor*>(userPointer);

        auto node = self->FindNode(nodeId);
        if (!node)
            return false;

        node->State.assign(data, size);

        self->TouchNode(nodeId);

        return true;
    };
    
    m_EditorContext = nodeEditor::CreateEditor(&config);
    nodeEditor::SetCurrentEditor(m_EditorContext);

    /*Node* node = new Node(1, "Sequencee");
    node = SpawnSequenceNode(ImVec2(0,0));
    //nodeEditor::SetNodePosition(node->ID, ImVec2(0,0));

    node = SpawnActionNode(ImVec2(-200, 100));

    node = SpawnSelectorNode(ImVec2(200, 100));*/

    nodeEditor::NavigateToContent();
    BuildNodes();

    /*m_Links.push_back(Link(GetNextLinkId(), m_Nodes[0].Outputs[0].ID, m_Nodes[1].Inputs[0].ID));
    m_Links.push_back(Link(GetNextLinkId(), m_Nodes[0].Outputs[0].ID, m_Nodes[2].Inputs[0].ID));*/
}

void NodeEditor::OnUpdate()
{
    ImGui::Separator();
    nodeEditor::SetCurrentEditor(m_EditorContext);
    nodeEditor::Begin("My nodeEditoritor", ImVec2(0.0, 0.0f));

    int uniqueId = 1;
        for (auto& node : m_Nodes)
            {
                const float rounding = 5.0f;
                const float padding  = 12.0f;

                const auto pinBackground = nodeEditor::GetStyle().Colors[nodeEditor::StyleColor_NodeBg];

                nodeEditor::PushStyleColor(nodeEditor::StyleColor_NodeBg,        ImColor(128, 128, 128, 200));
                nodeEditor::PushStyleColor(nodeEditor::StyleColor_NodeBorder,    ImColor( 32,  32,  32, 200));
                nodeEditor::PushStyleColor(nodeEditor::StyleColor_PinRect,       ImColor( 60, 180, 255, 150));
                nodeEditor::PushStyleColor(nodeEditor::StyleColor_PinRectBorder, ImColor( 60, 180, 255, 150));

                nodeEditor::PushStyleVar(nodeEditor::StyleVar_NodePadding,  ImVec4(0, 0, 0, 0));
                nodeEditor::PushStyleVar(nodeEditor::StyleVar_NodeRounding, rounding);
                nodeEditor::PushStyleVar(nodeEditor::StyleVar_SourceDirection, ImVec2(0.0f,  1.0f));
                nodeEditor::PushStyleVar(nodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
                nodeEditor::PushStyleVar(nodeEditor::StyleVar_LinkStrength, 0.0f);
                nodeEditor::PushStyleVar(nodeEditor::StyleVar_PinBorderWidth, 1.0f);
                nodeEditor::PushStyleVar(nodeEditor::StyleVar_PinRadius, 5.0f);
                nodeEditor::BeginNode(node.ID);
            
                ImGui::BeginVertical(node.ID.AsPointer());
                ImGui::BeginHorizontal("inputs");
                ImGui::Spring(0, padding * 2);

                ImRect inputsRect;
                int inputAlpha = 200;
                if (!node.Inputs.empty())
                {
                        auto& pin = node.Inputs[0];
                        ImGui::Dummy(ImVec2(0, padding));
                        ImGui::Spring(1, 0);
                        inputsRect = ax::NodeEditor::Detail::ImGui_GetItemRect();

                        nodeEditor::PushStyleVar(nodeEditor::StyleVar_PinArrowSize, 10.0f);
                        nodeEditor::PushStyleVar(nodeEditor::StyleVar_PinArrowWidth, 10.0f);
#if IMGUI_VERSION_NUM > 18101
                        nodeEditor::PushStyleVar(nodeEditor::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);
#else
                        nodeEditor::PushStyleVar(nodeEditor::StyleVar_PinCorners, 12);
#endif
                        nodeEditor::BeginPin(pin.ID, nodeEditor::PinKind::Input);
                        nodeEditor::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
                        nodeEditor::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
                        nodeEditor::EndPin();
                        nodeEditor::PopStyleVar(3);

                        if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                            inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
                }
                else
                    ImGui::Dummy(ImVec2(0, padding));

                ImGui::Spring(0, padding * 2);
                ImGui::EndHorizontal();

                ImGui::BeginHorizontal("content_frame");
                ImGui::Spring(1, padding);

                ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
                ImGui::Dummy(ImVec2(160, 0));
                ImGui::Spring(1);
                ImGui::TextUnformatted(node.Name.c_str());
                ImGui::Spring(1);
                ImGui::EndVertical();
                auto contentRect = ax::NodeEditor::Detail::ImGui_GetItemRect();

                ImGui::Spring(1, padding);
                ImGui::EndHorizontal();

                ImGui::BeginHorizontal("outputs");
                ImGui::Spring(0, padding * 2);

                ImRect outputsRect;
                int outputAlpha = 200;
                if (!node.Outputs.empty())
                {
                    auto& pin = node.Outputs[0];
                    ImGui::Dummy(ImVec2(0, padding));
                    ImGui::Spring(1, 0);
                    outputsRect = ax::NodeEditor::Detail::ImGui_GetItemRect();

#if IMGUI_VERSION_NUM > 18101
                    nodeEditor::PushStyleVar(nodeEditor::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);
#else
                    nodeEditor::PushStyleVar(nodeEditor::StyleVar_PinCorners, 3);
#endif
                    nodeEditor::BeginPin(pin.ID, nodeEditor::PinKind::Output);
                    nodeEditor::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
                    nodeEditor::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
                    nodeEditor::EndPin();
                    nodeEditor::PopStyleVar();

                    if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                        outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
                }
                else
                    ImGui::Dummy(ImVec2(0, padding));

                ImGui::Spring(0, padding * 2);
                ImGui::EndHorizontal();

                ImGui::EndVertical();

                nodeEditor::EndNode();
                nodeEditor::PopStyleVar(7);
                nodeEditor::PopStyleColor(4);

                auto drawList = nodeEditor::GetNodeBackgroundDrawList(node.ID);

                //const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
                //const auto unitSize    = 1.0f / fringeScale;

                //const auto ImDrawList_AddRect = [](ImDrawList* drawList, const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners, float thickness)
                //{
                //    if ((col >> 24) == 0)
                //        return;
                //    drawList->PathRect(a, b, rounding, rounding_corners);
                //    drawList->PathStroke(col, true, thickness);
                //};

#if IMGUI_VERSION_NUM > 18101
                const auto    topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
                const auto bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;
#else
                const auto    topRoundCornersFlags = 1 | 2;
                const auto bottomRoundCornersFlags = 4 | 8;
#endif

                drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
                //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
                //ImGui::PopStyleVar();
                drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);
                //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);
                //ImGui::PopStyleVar();
                drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
                //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                drawList->AddRect(
                    contentRect.GetTL(),
                    contentRect.GetBR(),
                    IM_COL32(48, 128, 255, 100), 0.0f);
                //ImGui::PopStyleVar();
            }
        for (auto& link : m_Links)
            nodeEditor::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);
        if (!createNewNode)
            {
                if (nodeEditor::BeginCreate(ImColor(255, 255, 255), 2.0f))
                {
                    auto showLabel = [](const char* label, ImColor color)
                    {
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                        auto size = ImGui::CalcTextSize(label);

                        auto padding = ImGui::GetStyle().FramePadding;
                        auto spacing = ImGui::GetStyle().ItemSpacing;

                        ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                        auto rectMin = ImGui::GetCursorScreenPos() - padding;
                        auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                        auto drawList = ImGui::GetWindowDrawList();
                        drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                        ImGui::TextUnformatted(label);
                    };

                    nodeEditor::PinId startPinId = 0, endPinId = 0;
                    if (nodeEditor::QueryNewLink(&startPinId, &endPinId))
                    {
                        auto startPin = FindPin(startPinId);
                        auto endPin   = FindPin(endPinId);

                        newLinkPin = startPin ? startPin : endPin;

                        if (startPin->Kind == PinKind::Input)
                        {
                            std::swap(startPin, endPin);
                            std::swap(startPinId, endPinId);
                        }

                        if (startPin && endPin)
                        {
                            if (endPin == startPin)
                            {
                                nodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                            }
                            else if (endPin->Kind == startPin->Kind)
                            {
                                showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                                nodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                            }
                            //else if (endPin->Node == startPin->Node)
                            //{
                            //    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                            //    nodeEditor::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                            //}
                            /*else if (endPin->Type != startPin->Type)
                            {
                                showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                                nodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                            }*/
                            else
                            {
                                showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                                if (nodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                                {
                                    if (startPinId == m_RootOutputPinId)
                                    {
                                        auto rootOutputPinId = startPinId;
                                        for (int i = 0; i < (int)m_Links.size(); ++i)
                                            if (m_Links[i].StartPinID == rootOutputPinId)
                                            {
                                                m_Links.erase(m_Links.begin() + i);
                                                break;
                                            }
                                    }
                                    m_Links.emplace_back(Link(GetNextID(), startPinId, endPinId));
                                    m_Links.back().Color = ImColor(255, 255, 255);/*GetIconColor(PinType::Flow);*/
                                }
                            }
                        }
                    }

                    nodeEditor::PinId pinId = 0;
                    if (nodeEditor::QueryNewNode(&pinId))
                    {
                        newLinkPin = FindPin(pinId);
                        if (newLinkPin)
                            showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                        if (nodeEditor::AcceptNewItem())
                        {
                            createNewNode  = true;
                            newNodeLinkPin = FindPin(pinId);
                            newLinkPin = nullptr;
                            nodeEditor::Suspend();
                            ImGui::OpenPopup("Create New Node");
                            nodeEditor::Resume();
                        }
                    }
                }
                else
                    newLinkPin = nullptr;

                nodeEditor::EndCreate();

                if (nodeEditor::BeginDelete())
                {
                    nodeEditor::NodeId nodeId = 0;
                    while (nodeEditor::QueryDeletedNode(&nodeId))
                    {
                        if (nodeEditor::AcceptDeletedItem())
                        {
                            auto id = std::find_if(m_Nodes.begin(), m_Nodes.end(), [nodeId](auto& node) { return node.ID == nodeId; });
                            if (id != m_Nodes.end())
                                m_Nodes.erase(id);
                        }
                    }

                    nodeEditor::LinkId linkId = 0;
                    while (nodeEditor::QueryDeletedLink(&linkId))
                    {
                        if (nodeEditor::AcceptDeletedItem())
                        {
                            auto id = std::find_if(m_Links.begin(), m_Links.end(), [linkId](auto& link) { return link.ID == linkId; });
                            if (id != m_Links.end())
                                m_Links.erase(id);
                        }
                    }
                }
                nodeEditor::EndDelete();
            }
    /*for (auto& link : m_Links)
    {
        nodeEditor::Flow(link.ID);
    }*/
    nodeEditor::End();
    //nodeEditor::SetCurrentEditor(nullptr);
}

void NodeEditor::BuildNode(Node* node)
{
    for (auto& input : node->Inputs)
    {
        input.Node = node;
        input.Kind = PinKind::Input;
    }

    for (auto& output : node->Outputs)
    {
        output.Node = node;
        output.Kind = PinKind::Output;
    }
}

Node* NodeEditor::SpawnRootNode()
{
    m_Nodes.emplace_back(GetNextID(), "Root");
    //m_Nodes.back().Type = NodeType::Tree;
    m_Nodes.back().Outputs.emplace_back(GetNextID(), "");
    nodeEditor::SetNodePosition(m_Nodes.back().ID, ImVec2(0, 0));
    BuildNode(&m_Nodes.back());
    m_RootOutputPinId = m_Nodes.back().Outputs[0].ID;
    return &m_Nodes.back();
}

Node* NodeEditor::SpawnSequenceNode(ImVec2 position)
{
    m_Nodes.emplace_back(GetNextID(), "Sequence");
    //m_Nodes.back().Type = NodeType::Tree;
    m_Nodes.back().Inputs.emplace_back(GetNextID(), "");
    m_Nodes.back().Outputs.emplace_back(GetNextID(), "");
    nodeEditor::SetNodePosition(m_Nodes.back().ID, position);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node* NodeEditor::SpawnSelectorNode(ImVec2 position)
{
    m_Nodes.emplace_back(GetNextID(), "Selector");
    //m_Nodes.back().Type = NodeType::Tree;
    m_Nodes.back().Inputs.emplace_back(GetNextID(), "");
    m_Nodes.back().Outputs.emplace_back(GetNextID(), "");
    nodeEditor::SetNodePosition(m_Nodes.back().ID, position);
    BuildNode(&m_Nodes.back());
    return &m_Nodes.back();
}

Node* NodeEditor::SpawnActionNode(ImVec2 position)
{
    m_Nodes.emplace_back(GetNextID(), "Action");
    //m_Nodes.back().Type = NodeType::Action;
    m_Nodes.back().Inputs.emplace_back(GetNextID(), "");
    nodeEditor::SetNodePosition(m_Nodes.back().ID, position);
    BuildNode(&m_Nodes.back());
    return &m_Nodes.back();
}

Node* NodeEditor::SpawnConditionNode()
{
    m_Nodes.emplace_back(GetNextID(), "Condition");
    //m_Nodes.back().Type = NodeType::Condition;
    m_Nodes.back().Outputs.emplace_back(GetNextID(), "");
    BuildNode(&m_Nodes.back());
    return &m_Nodes.back();
}

Node* NodeEditor::SpawnDecoratorNode()
{
    m_Nodes.emplace_back(GetNextID(), "Decorator");
    //m_Nodes.back().Type = NodeType::Decorator;
    m_Nodes.back().Inputs.emplace_back(GetNextID(), "");
    m_Nodes.back().Outputs.emplace_back(GetNextID(), "");
    BuildNode(&m_Nodes.back());
    return &m_Nodes.back();
}
