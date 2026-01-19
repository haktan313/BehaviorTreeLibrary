#pragma once
#include <unordered_map>
#include <vector>
#include <xstring>

class BehaviorTree;

class Root
{
public:
    static void RootStart();
    static void RootTick();
    static void RootClear();
    static void RootStop();

    static BehaviorTree* CreateBehaviorTree(const std::string& name);
    static BehaviorTree* CreateBehaviorTree(const std::string& name, const std::string& path);
    static void DestroyBehaviorTree(BehaviorTree* tree);

    static std::vector<BehaviorTree*>& GetBehaviorTrees() { return m_BehaviorTrees; }
    static std::string GetBehaviorTreePath(BehaviorTree* tree);
private:
    static std::vector<BehaviorTree*> m_BehaviorTrees;
    static std::unordered_map<BehaviorTree*, std::string> m_BehaviorTreeMap;
};
