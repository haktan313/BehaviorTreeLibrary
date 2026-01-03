#pragma once
#include "Tree.h"
#include "yaml-cpp/emitter.h"

class BTSerializer
{
public:
    BTSerializer(const BehaviorTree& tree);
    ~BTSerializer() = default;

    void Serialize(const std::string& filepath);
    bool Deserialize(const std::string& filepath);
private:
    static const char* NodeTypeToString(HNodeType type);
    static const char* PriorityToString(PriorityType p);
    
    static void SerializeChildren(YAML::Emitter& out, const HNode* node);
    static void SerializeNode(YAML::Emitter& out, const HNode* node);
    static void SerializeConditions(YAML::Emitter& out, const HNode* node);

    const BehaviorTree* m_Tree;
};
