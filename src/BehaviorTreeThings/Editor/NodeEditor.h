#pragma once
#include "NodeEditorStructsAndEnums.h"


class NodeEditor
{
public:
    NodeEditor();
    
    static Node* FindNode(nodeEditor::NodeId id);
    static void TouchNode(nodeEditor::NodeId id);
    static void BuildNodes();
    static bool CanCreateLink(Pin* a, Pin* b);
    static Pin* FindPin(nodeEditor::PinId id);
    static int GetNextID();
    static ax::NodeEditor::LinkId GetNextLinkId() { return nodeEditor::LinkId(GetNextID()); }
    static std::vector<Node>& GetNodes() { return m_Nodes; }
    static std::vector<Link>& GetLinks() { return m_Links; }
    static void OnStart();
    static void OnUpdate();
    static void BuildNode(Node* node);

    static Node* SpawnRootNode();
    static Node* SpawnSequenceNode(ImVec2 position);
    static Node* SpawnSelectorNode(ImVec2 position);
    static Node* SpawnActionNode(ImVec2 position);
    static Node* SpawnConditionNode();
    static Node* SpawnDecoratorNode();
private:
    static nodeEditor::PinId m_RootOutputPinId;
    static std::vector<Node> m_Nodes;
    static std::vector<Link> m_Links;
    static std::map<nodeEditor::NodeId, float, NodeIdLess> m_NodeTouchTime;
    static NodeEditor* s_Instance;
};
