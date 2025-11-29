#pragma once

#include <imgui_node_editor_internal.h>

namespace nodeEditor = ax::NodeEditor;

/*enum class PinType
{
    Flow
};*/

enum class PinKind
{
    Output,
    Input
};

/*enum class NodeType
{
    Tree
};*/

struct Node;
struct Pin
{
    nodeEditor::PinId   ID;
    Node*     Node;
    std::string Name;
    //PinType     Type;
    PinKind     Kind;

    Pin(int id, const char* name/*, PinType type*/):
        ID(id), Node(nullptr), Name(name),/* Type(type),*/ Kind(PinKind::Input)
    {
    }
};

struct Node
{
    nodeEditor::NodeId ID;
    std::string Name;
    std::vector<Pin> Inputs;
    std::vector<Pin> Outputs;
    ImColor Color;
    //NodeType Type;
    ImVec2 Size;

    std::string State;
    std::string SavnodeEditorState;

    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)):
        ID(id), Name(name), Color(color), /*Type(NodeType::Tree),*/ Size(0, 0)
    {
    }
};

struct Link
{
    nodeEditor::LinkId ID;

    nodeEditor::PinId StartPinID;
    nodeEditor::PinId EndPinID;

    ImColor Color;

    Link(nodeEditor::LinkId id, nodeEditor::PinId startPinId, nodeEditor::PinId endPinId):
        ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
    {
    }
};

struct NodeIdLess
{
    bool operator()(const nodeEditor::NodeId& lhs, const nodeEditor::NodeId& rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

enum class PinType
{
    Flow,
    Bool,
    Int,
    Float,
    String,
    Object,
    Function,
    Delegate,
};
