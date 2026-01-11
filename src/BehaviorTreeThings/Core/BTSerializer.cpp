#include "BTSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "NodeRegistry.h"

BTSerializer::BTSerializer(BehaviorTree*& tree) : m_Tree(tree)
{
}

void BTSerializer::Serialize(const std::string& filepath)
{
    auto rootNode = m_Tree->GetRootNode();

    YAML::Emitter out;
    out << YAML::BeginMap;

    out << YAML::Key << "BehaviorTree" << YAML::Value;
    out << YAML::BeginMap;

    out << YAML::Key << "Blackboard" << YAML::Value;
    out << YAML::BeginMap;
    SerializeBlackboard(out, m_Tree->GetBlackboard());
    out << YAML::EndMap;

    out << YAML::Key << "EditorData" << YAML::Value;
    out << YAML::BeginMap;
    SerializeEditorData(out);
    out << YAML::EndMap;

    out << YAML::Key << "RootNode" << YAML::Value;
    SerializeNode(out, rootNode);

    out << YAML::EndMap;// BehaviorTree
    out << YAML::EndMap;// root

    std::ofstream fout(filepath);
    fout << out.c_str();
}

bool BTSerializer::Deserialize(const std::string& filepath/*, EnemyAI& owner*/)
{
    YAML::Node data;
    try {
        data = YAML::LoadFile(filepath);
    }
    catch (const std::exception& e)
    {
        return false;
    }

    auto btNode = data["BehaviorTree"];
    if (!btNode)
        return false;
    auto editorApp = Root::GetNodeEditorApp();

    /*HBlackboard* blackboard = new HBlackboard();
    DeserializeBlackboard(btNode["Blackboard"], blackboard);*/
    HBlackboard* blackboard = nullptr;
    std::string bbClassName = "";

    if (btNode["Blackboard"]["ClassName"])
    {
        bbClassName = btNode["Blackboard"]["ClassName"].as<std::string>();
        auto& bbRegistry = NodeRegistry::GetBlackboardClassInfoMap();
        
        auto it = bbRegistry.find(bbClassName);
        if (it != bbRegistry.end())
        {
            if (editorApp)
            {
                blackboard = &Root::GetNodeEditorApp()->SetBlackboardForEditor(bbClassName, it->second);
                std::cout << "Created blackboard of class by Editor: " << bbClassName << std::endl;
            }
            else
            {
                blackboard = it->second.CreateBlackboardFn().release();
                std::cout << "Created blackboard of class: " << bbClassName << std::endl;
            }
        }
    }
    if (!blackboard)
        blackboard = new HBlackboard();
    DeserializeBlackboard(btNode["Blackboard"], blackboard);
    
    
    if (!editorApp)
    {
        BehaviorTreeBuilder builder/*(&owner)*/;
        builder.setBlackboard(blackboard);

        DeserializeNodeRecursive(btNode["RootNode"], builder);

        m_Tree = builder.build();
        return true;
    }
    editorApp->ClearBuildData();
    Root::GetNodeEditorApp()->GetNodeEditorHelper().ClearDatas();
    
    std::unordered_map<int, nodeEditor::NodeId> idMap;

    int maxID = 0;

    if (btNode["EditorData"]["Nodes"])
    {
        for (auto n : btNode["EditorData"]["Nodes"])
        {
            int oldID = n["ID"].as<int>();
            maxID = std::max(maxID, oldID);
            
            std::string name = n["Name"].as<std::string>();
            NodeType type = static_cast<NodeType>(n["Type"].as<int>());
            ImVec2 pos(n["PosX"].as<float>(), n["PosY"].as<float>());

            Node* newNode = nullptr;

            switch (type)
            {
                case NodeType::Root:
                    newNode = Root::GetNodeEditorApp()->GetNodeEditorHelper().SpawnRootNode();
                break;
                case NodeType::Sequence:
                    newNode = Root::GetNodeEditorApp()->GetNodeEditorHelper().SpawnSequenceNode(pos);
                break;
                case NodeType::Selector:
                    newNode = Root::GetNodeEditorApp()->GetNodeEditorHelper().SpawnSelectorNode(pos);
                break;
                case NodeType::Action:
                    newNode = Root::GetNodeEditorApp()->GetNodeEditorHelper().SpawnActionNode(pos);
                break;
            }

            if (newNode)
            {
                newNode->Name = name;
                nodeEditor::SetNodePosition(newNode->ID, pos);
                idMap[oldID] = newNode->ID; 
            }
        }
    }
    Root::GetNodeEditorApp()->GetNodeEditorHelper().BuildNodes();
    if (btNode["EditorData"]["Links"])
    {
        auto& helper = Root::GetNodeEditorApp()->GetNodeEditorHelper();
        for (auto l : btNode["EditorData"]["Links"])
        {
            int startNodeOldID = l["StartNodeID"].as<int>();
            int endNodeOldID = l["EndNodeID"].as<int>();
            
            auto itA = idMap.find(startNodeOldID);
            auto itB = idMap.find(endNodeOldID);
            if (itA == idMap.end() || itB == idMap.end())
                continue;

            Node* startNode = helper.FindNode(itA->second);
            Node* endNode   = helper.FindNode(itB->second);

            if (!startNode || !endNode)
                continue;

            if (!startNode->Outputs.empty() && !endNode->Inputs.empty())
            {
                nodeEditor::PinId startPin = startNode->Outputs[0].ID;
                nodeEditor::PinId endPin   = endNode->Inputs[0].ID;
                helper.GetLinks().emplace_back(Link(helper.GetNextLinkId(), startPin, endPin));
            }

        }
    }
    
    return true;
}

const char* BTSerializer::NodeTypeToString(HNodeType type)
{
    switch (type)
    {
        case HNodeType::Root:
            return "Root";
        case HNodeType::Composite:
            return "Composite";
        case HNodeType::Action:
            return "Action";
        case HNodeType::Condition:
            return "Condition";
        case HNodeType::Decorator:
            return "Decorator";
        default:
            return "Unknown";
    }
}

const char* BTSerializer::PriorityToString(PriorityType p)
{
    switch (p)
    {
        case PriorityType::None:
            return "None";
        case PriorityType::Self:
            return "Self";
        case PriorityType::LowerPriority:
            return "LowerPriority";
        case PriorityType::Both:
            return "Both";
        default:
            return "None";
    }
}

void BTSerializer::SerializeBlackboard(YAML::Emitter& out, const HBlackboard* blackboard)
{
    auto classInfo = NodeRegistry::GetBlackboardClassInfoMap().find(blackboard->GetName());
    if (classInfo != NodeRegistry::GetBlackboardClassInfoMap().end())
        out << YAML::Key << "ClassName" << YAML::Value << classInfo->second.Name;
    
    out << YAML::Key << "Floats" << YAML::Value << YAML::BeginMap;
    for (const auto& [key, val] : blackboard->GetFloatValues())
        out << YAML::Key << key << YAML::Value << val;
    out << YAML::EndMap;
    
    out << YAML::Key << "Ints" << YAML::Value << YAML::BeginMap;
    for (const auto& [key, val] : blackboard->GetIntValues())
        out << YAML::Key << key << YAML::Value << val;
    out << YAML::EndMap;
    
    out << YAML::Key << "Bools" << YAML::Value << YAML::BeginMap;
    for (const auto& [key, val] : blackboard->GetBoolValues())
        out << YAML::Key << key << YAML::Value << val;
    out << YAML::EndMap;
    
    out << YAML::Key << "Strings" << YAML::Value << YAML::BeginMap;
    for (const auto& [key, val] : blackboard->GetStringValues())
        out << YAML::Key << key << YAML::Value << val;
    out << YAML::EndMap;
}

void BTSerializer::DeserializeBlackboard(const YAML::Node& blackboardNode, HBlackboard* blackboard)
{
    if (!blackboardNode || !blackboard)
        return;
    
    if (blackboardNode["Floats"])
        for (auto it = blackboardNode["Floats"].begin(); it != blackboardNode["Floats"].end(); ++it)
        {
            const std::string key = it->first.as<std::string>();
            const float value = it->second.as<float>();

            if (blackboard->HasFloatValue(key))
                blackboard->SetFloatValue(key, value);
            else
                blackboard->CreateFloatValue(key, value);
        }

    if (blackboardNode["Ints"])
        for (auto it = blackboardNode["Ints"].begin(); it != blackboardNode["Ints"].end(); ++it)
        {
            const std::string key = it->first.as<std::string>();
            const int value = it->second.as<int>();

            if (blackboard->HasIntValue(key))
                blackboard->SetIntValue(key, value);
            else
                blackboard->CreateIntValue(key, value);
        }

    if (blackboardNode["Bools"])
        for (auto it = blackboardNode["Bools"].begin(); it != blackboardNode["Bools"].end(); ++it)
        {
            const std::string key = it->first.as<std::string>();
            const bool value = it->second.as<bool>();

            if (blackboard->HasBoolValue(key))
                blackboard->SetBoolValue(key, value);
            else
                blackboard->CreateBoolValue(key, value);
        }

    if (blackboardNode["Strings"])
        for (auto it = blackboardNode["Strings"].begin(); it != blackboardNode["Strings"].end(); ++it)
        {
            const std::string key = it->first.as<std::string>();
            const std::string value = it->second.as<std::string>();

            if (blackboard->HasStringValue(key))
                blackboard->SetStringValue(key, value);
            else
                blackboard->CreateStringValue(key, value);
        }
}

void BTSerializer::SerializeEditorData(YAML::Emitter& out)
{
    auto editorApp = Root::GetNodeEditorApp();
    if (!editorApp)
        return;

    auto& helper = editorApp->GetNodeEditorHelper();
    
    out << YAML::Key << "Nodes" << YAML::Value << YAML::BeginSeq;
    for (const auto& node : helper.GetNodes())
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ID" << YAML::Value << node.ID.Get();
        out << YAML::Key << "Name" << YAML::Value << node.Name;
        out << YAML::Key << "Type" << YAML::Value << static_cast<int>(node.Type);
        
        ImVec2 pos = nodeEditor::GetNodePosition(node.ID);
        out << YAML::Key << "PosX" << YAML::Value << pos.x;
        out << YAML::Key << "PosY" << YAML::Value << pos.y;
        
        out << YAML::Key << "Decorators" << YAML::Value << YAML::BeginSeq;
        for(const auto& deco : node.Decorators)
            out << deco.Name;
        out << YAML::EndSeq;

        out << YAML::Key << "Conditions" << YAML::Value << YAML::BeginSeq;
        for(const auto& cond : node.Conditions)
            out << cond.Name;
        out << YAML::EndSeq;
        
        int nodeKey = (int)node.ID.Get();
        out << YAML::Key << "Params" << YAML::Value << YAML::BeginMap;
        //editorApp->GetParamsForNode(nodeKey)->Serialize(out);
        out << YAML::EndMap;

        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
    
    out << YAML::Key << "Links" << YAML::Value << YAML::BeginSeq;
    for (const auto& link : helper.GetLinks())
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ID" << YAML::Value << link.ID.Get();
        
        auto* startPin = helper.FindPin(link.StartPinID);
        auto* endPin = helper.FindPin(link.EndPinID);
        
        if (startPin && endPin)
        {
            out << YAML::Key << "StartNodeID" << YAML::Value << startPin->Node->ID.Get();
            out << YAML::Key << "EndNodeID" << YAML::Value << endPin->Node->ID.Get();
        }
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
}


void BTSerializer::SerializeChildren(YAML::Emitter& out, const HNode* node)
{
    auto children = node->GetChildrensRaw();

    out << YAML::Key << "Children" << YAML::Value;
    out << YAML::BeginSeq;

    for (const auto* child : children)
    {
        SerializeNode(out, child);
    }

    out << YAML::EndSeq;
}

void BTSerializer::SerializeNode(YAML::Emitter& out, const HNode* node)
{
    if (!node)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "NullNode" << YAML::Value << true;
        out << YAML::EndMap;
        return;
    }

    out << YAML::BeginMap;
    
    out << YAML::Key << "Name"  << YAML::Value << node->GetName();
    out << YAML::Key << "Type"  << YAML::Value << NodeTypeToString(node->GetType());
    
    out << YAML::Key << "Class" << YAML::Value << typeid(*node).name();
    SerializeConditions(out, node);

    if (node->GetType() == HNodeType::Decorator)
    {
        std::cout << "DEC name=" << node->GetName() << " class=" << typeid(*node).name() << "\n";
        auto children = node->GetChildrensRaw();

        out << YAML::Key << "Child" << YAML::Value;

        if (!children.empty() && children[0])
            SerializeNode(out, children[0]);
        else
        {
            out << YAML::BeginMap;
            out << YAML::Key << "NullNode" << YAML::Value << true;
            out << YAML::EndMap;
        }
    }
    else if (node->GetType() == HNodeType::Root || node->GetType() == HNodeType::Composite)
    {
        SerializeChildren(out, node);
    }

    if (node->GetType() == HNodeType::Condition)
    {
        std::cout << "COND name=" << node->GetName() << " class=" << typeid(*node).name() << "\n";
        auto* cond = dynamic_cast<const HCondition*>(node);
        if (cond)
            out << YAML::Key << "Priority" << YAML::Value << PriorityToString(cond->GetPriorityMode());
    }

    out << YAML::Key << "Params" << YAML::Value;
    out << YAML::BeginMap;
    node->GetParams().Serialize(out);
    out << YAML::EndMap;

    out << YAML::EndMap;
}

void BTSerializer::DeserializeNodeRecursive(const YAML::Node& nodeData, BehaviorTreeBuilder& builder)
{
    if (!nodeData || nodeData["NullNode"])
        return;

    std::string name = nodeData["Name"].as<std::string>();
    std::string type = nodeData["Type"].as<std::string>();
    std::string className = nodeData["Class"].as<std::string>();

    if (type == "Root")
    {
        builder.root(nullptr);
        if (nodeData["Children"]) 
            for (auto child : nodeData["Children"])
                DeserializeNodeRecursive(child, builder);
    }
    else if (type == "Decorator")
    {
        auto& decoMap = NodeRegistry::GetDecoratorClassInfoMap();
        auto it = decoMap.find(name);
        
        if (it != decoMap.end())
        {
            const YAML::Node& paramsNode = nodeData["Params"];
            it->second.BuildFromYAML(builder, name, paramsNode);
        }
        
        if (nodeData["Child"])
            DeserializeNodeRecursive(nodeData["Child"], builder);
    }
    else if (type == "Composite")
    {
        if (className.find("Selector") != std::string::npos)
            builder.selector(name);
        else
            builder.sequence(name);
        if (nodeData["Children"])
            for (auto child : nodeData["Children"])
                DeserializeNodeRecursive(child, builder);
        builder.end();
    }
    else if (type == "Action")
    {
        auto& actionMap = NodeRegistry::GetActionClassInfoMap();
        auto it = actionMap.find(name);
        if (it != actionMap.end())
        {
            const ActionClassInfo& actionInfo = it->second;
            if (actionInfo.BuildFromYAML)
            {
                const YAML::Node& paramsNode = nodeData["Params"];
                actionInfo.BuildFromYAML(builder, name, paramsNode);
            }
        }
    }

    if (nodeData["Conditions"])
        for (auto condData : nodeData["Conditions"])
            DeserializeCondition(condData, builder);
}

void BTSerializer::SerializeConditions(YAML::Emitter& out, const HNode* node)
{
    auto conditions = node->GetConditionNodesRaw();

    if (conditions.empty())
        return;

    out << YAML::Key << "Conditions" << YAML::Value;
    out << YAML::BeginSeq;

    for (const auto* cond : conditions)
        SerializeNode(out, cond);

    out << YAML::EndSeq;
}

void BTSerializer::DeserializeCondition(const YAML::Node& condData, BehaviorTreeBuilder& builder)
{
    std::string name = condData["Name"].as<std::string>();
    auto& condMap = NodeRegistry::GetConditionClassInfoMap();
    auto it = condMap.find(name);

    if (it != condMap.end())
    {
        PriorityType priority = PriorityType::None;
        if (condData["Priority"])
        {
            std::string pStr = condData["Priority"].as<std::string>();
            if (pStr == "Self")
                priority = PriorityType::Self;
            else if (pStr == "LowerPriority")
                priority = PriorityType::LowerPriority;
            else if (pStr == "Both")
                priority = PriorityType::Both;
        }

        const YAML::Node& paramsNode = condData["Params"];
        it->second.BuildFromYAML(builder, name, paramsNode, priority);
    }
}
