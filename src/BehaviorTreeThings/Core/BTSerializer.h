#pragma once
#include <functional>

#include "Tree.h"
#include "yaml-cpp/emitter.h"

class BTSerializer
{
public:
    BTSerializer(const BehaviorTree& tree);
    ~BTSerializer() = default;

    void Serialize(const std::string& filepath);
    bool Deserialize(const std::string& filepath/*, EnemyAI& owner*/);
private:
    static const char* NodeTypeToString(HNodeType type);
    static const char* PriorityToString(PriorityType p);

    static void SerializeBlackboard(YAML::Emitter& out, const HBlackboard* blackboard);
    static void DeserializeBlackboard(const YAML::Node& blackboardNode, HBlackboard* blackboard);

    static void SerializeChildren(YAML::Emitter& out, const HNode* node);
    static void SerializeNode(YAML::Emitter& out, const HNode* node);
    static void DeserializeNodeRecursive(const YAML::Node& nodeData, BehaviorTreeBuilder& builder);
    static void SerializeConditions(YAML::Emitter& out, const HNode* node);

    const BehaviorTree* m_Tree;
};