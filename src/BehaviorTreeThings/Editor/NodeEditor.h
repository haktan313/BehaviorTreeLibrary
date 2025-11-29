#pragma once
#include "NodeEditorStructsAndEnums.h"


class NodeEditor
{
public:
    Node* FindNode(nodeEditor::NodeId id);
    void TouchNode(nodeEditor::NodeId id);
    void BuildNodes();
    bool CanCreateLink(Pin* a, Pin* b);
    Pin* FindPin(nodeEditor::PinId id);
    int GetNextID() { return m_NextId++; }
    ax::NodeEditor::LinkId GetNextLinkId() { return nodeEditor::LinkId(GetNextID()); }
    void OnStart();
    void OnUpdate();
private:
    void BuildNode(Node* node);
    inline int GetNextId() { return m_NextId++; }
    
    Node* SpawnSequenceNode();
    Node* SpawnSelectorNode();
    Node* SpawnActionNode();
    Node* SpawnConditionNode();
    Node* SpawnDecoratorNode();

    nodeEditor::EditorContext* m_EditorContext = nullptr;
    std::vector<Node> m_Nodes;
    std::vector<Link> m_Links;
    int m_NextId = 1;
    std::map<nodeEditor::NodeId, float, NodeIdLess> m_NodeTouchTime;
    const float          m_TouchTime = 1.0f;
};
