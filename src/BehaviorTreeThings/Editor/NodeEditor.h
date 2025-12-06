#pragma once
#include "NodeEditorStructsAndEnums.h"

class NodeEditorApp;
class NodeEditor
{
    NodeEditorApp* m_App;
public:
    NodeEditor(NodeEditorApp* app = nullptr);
    
    Node* FindNode(nodeEditor::NodeId id);
    void TouchNode(nodeEditor::NodeId id);
    Pin* FindPin(nodeEditor::PinId id);
    int GetNextID();
    ax::NodeEditor::LinkId GetNextLinkId() { return nodeEditor::LinkId(GetNextID()); }
    std::vector<Node>& GetNodes() { return m_Nodes; }
    std::vector<Link>& GetLinks() { return m_Links; }
    Node* GetSelectedNode();
    void OnStart();
    void OnUpdate();

    Node* SpawnRootNode();
    Node* SpawnSequenceNode(ImVec2 position);
    Node* SpawnSelectorNode(ImVec2 position);
    Node* SpawnActionNode(ImVec2 position);
    void SpawnConditionNode(Node* parentNode);
    void SpawnDecoratorNode(Node* parentNode);
    std::vector<Node*> GetChilderenNodes(Node* parentNode);
private:
    void BuildNode(Node* node);
public:
    void BuildNodes();
private:
    void StylizeNodes();
    void ManageInputs(ImRect& inputsRect, int& inputAlpha, Node& node, float padding);
    void DrawDecoratorBar(Node& node, ImRect& decoratorRect);
    void DrawConditionBar(Node& node, ImRect& conditionRect);
    void ManageOutputs(ImRect& outputsRect, int& outputAlpha, Node& node, float padding);
    void PaintNodeBackground(Node& node, const ImRect& inputsRect, const ImRect& outputsRect, const ImRect& contentRect, const ImVec4& pinBackground, int inputAlpha, int outputAlpha, const ImRect& sequenceRect);
    void ManageLinks();
    bool CanCreateLink(Pin* a, Pin* b);
    
    nodeEditor::PinId m_RootOutputPinId;
    std::vector<Node> m_Nodes;
    std::vector<Link> m_Links;
    std::map<nodeEditor::NodeId, float, NodeIdLess> m_NodeTouchTime;
    //static NodeEditor* s_Instance;
};
