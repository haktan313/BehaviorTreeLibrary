#pragma once
#include "NodeEditorStructsAndEnums.h"


class NodeEditor
{
public:
    NodeEditor();
    
    static Node* FindNode(nodeEditor::NodeId id);
    static void TouchNode(nodeEditor::NodeId id);
    static Pin* FindPin(nodeEditor::PinId id);
    static int GetNextID();
    static ax::NodeEditor::LinkId GetNextLinkId() { return nodeEditor::LinkId(GetNextID()); }
    static std::vector<Node>& GetNodes() { return m_Nodes; }
    static std::vector<Link>& GetLinks() { return m_Links; }
    static void OnStart();
    static void OnUpdate();

    static Node* SpawnRootNode();
    static Node* SpawnSequenceNode(ImVec2 position);
    static Node* SpawnSelectorNode(ImVec2 position);
    static Node* SpawnActionNode(ImVec2 position);
    static Node* SpawnConditionNode();
    static Node* SpawnDecoratorNode();
    static std::vector<Node*> GetChilderenNodes(Node* parentNode);
private:

    static void BuildNode(Node* node);
public:
    static void BuildNodes();
private:
    static void StylizeNodes();
    static void ManageInputs(ImRect& inputsRect, int& inputAlpha, Node& node, float padding);
    static void ManageOutputs(ImRect& outputsRect, int& outputAlpha, Node& node, float padding);
    static void PaintNodeBackground(Node& node, const ImRect& inputsRect, const ImRect& outputsRect, const ImRect& contentRect, const ImVec4& pinBackground, int
                                    inputAlpha, int outputAlpha);
    static void ManageLinks();
    static bool CanCreateLink(Pin* a, Pin* b);
    
    static nodeEditor::PinId m_RootOutputPinId;
    static std::vector<Node> m_Nodes;
    static std::vector<Link> m_Links;
    static std::map<nodeEditor::NodeId, float, NodeIdLess> m_NodeTouchTime;
    static NodeEditor* s_Instance;
};
