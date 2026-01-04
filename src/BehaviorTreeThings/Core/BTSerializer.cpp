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

    HBlackboard* blackboard = new HBlackboard();
    DeserializeBlackboard(btNode["Blackboard"], blackboard);
    
    BehaviorTreeBuilder builder/*(&owner)*/;
    builder.setBlackboard(blackboard);

    DeserializeNodeRecursive(btNode["RootNode"], builder);

    m_Tree = builder.build();
    
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
            blackboard->CreateFloatValue(it->first.as<std::string>(), it->second.as<float>());
    
    if (blackboardNode["Ints"])
        for (auto it = blackboardNode["Ints"].begin(); it != blackboardNode["Ints"].end(); ++it)
            blackboard->CreateIntValue(it->first.as<std::string>(), it->second.as<int>());
    
    if (blackboardNode["Bools"])
        for (auto it = blackboardNode["Bools"].begin(); it != blackboardNode["Bools"].end(); ++it)
            blackboard->CreateBoolValue(it->first.as<std::string>(), it->second.as<bool>());
    
    if (blackboardNode["Strings"])
        for (auto it = blackboardNode["Strings"].begin(); it != blackboardNode["Strings"].end(); ++it)
            blackboard->CreateStringValue(it->first.as<std::string>(), it->second.as<std::string>());
}

void BTSerializer::SerializeEditorData(YAML::Emitter& out)
{
    auto editorApp = Root::GetNodeEditorApp();
    if (!editorApp)
        return;
    auto editorData = editorApp->GetNodeMappings();
    auto helper = editorApp->GetNodeEditorHelper();
    for (const auto& [runtimeNode, editorId] : editorData)
    {
        out << YAML::Key << runtimeNode->GetName() << YAML::Value;
        out << YAML::BeginMap;
        ImVec2 pos = nodeEditor::GetNodePosition(editorId);
        out << YAML::Key << "PositionX" << YAML::Value << pos.x;
        out << YAML::Key << "PositionY" << YAML::Value << pos.y;
        out << YAML::EndMap;
    }
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
