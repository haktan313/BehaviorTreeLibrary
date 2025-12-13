# Behavior Tree Library
Behavior Tree library provides a framework in C++ with visual and script based tree creation, blackboards, actions, decorators, conditions, and live node flow debugging.
The node visuals are created using **imgui-node-editor**. You can find **example implementations** in the **CustomThings** folder.
- For cloning repo `git clone https://github.com/haktan313/NavigationmeshSystem.git`
- Then run the Generation.bat file

## Features
- Visual **Behavior Tree editor** built with **ImGui** + **imgui-node-editor**
- Script based and visual **Behavior Tree** creation in editor window
- Extensible system with **custom Actions**, **Decorators**, and **Conditions**
- **Blackboard** driven AI architecture
- Runtime node execution **flow visualization** (live active node & link flow)
- **Condition priority system** (Self, Lower Priority, Both)
- **Decorator execution control** (cooldown, result override, etc.)
- Fully C++ based framework with clear node lifecycle (OnStart / Tick / OnFinish / CheckCondition / CanExecute / OnFinishedResult / OnAbort)
- Ability to assign custom blackboards, actions, decorators, and conditions directly in the editor
- Designed for game AI and experimentation

## Screenshots
| Visual Behavior Tree Creation | Script Base Behavior Tree Creation |
|--------|-------------|
| <img width="1917" height="1106" alt="image" src="https://github.com/user-attachments/assets/98c4969a-c7ec-42c1-a17a-a4ff5035c304" /> | <img width="1520" height="902" alt="image" src="https://github.com/user-attachments/assets/bab44b5e-99bc-422b-848e-dfa7a22fa1d5" /> |


## Usage Examples

### Creating a Blackboard
- Create a class derived from the **HBlackboard** base class.
- In its constructor, create blackboard values using the **CreateXValue** functions by assigning a **KeyName** and a **value**.
- After creating your **blackboard**, to register it to the **editor**, go to `NodeEditorApp.cpp` and call `AddBlackBoardToEditor<YourBlackboardClass>(Its name)` inside the **OnStart** function:
  ```cpp
  AddBlackBoardToEditor<MeleeEnemyBlackboard>("Melee Enemy Blackboard");
  ```
  ### HBlackboard
  - **HBlackboard** has its own **DrawImGui** function, allowing it to be visualized directly in the editor.
  - Provides **Get** and **Set** functions for accessing **blackboard values**. Also includes helper **Get** functions to **retrieve all variables of the same type**.
  - There is a **DrawImGui** function, with it you will see the varaibles visual at the editor.
  - When a value changes, an internal flag is set to true for **condition checking**. This allows the tree to skip condition checks unless values change, improving performance.

    ```cpp
    class MeleeEnemyBlackboard : public HBlackboard
    {
    public:
        MeleeEnemyBlackboard()
        {
            CreateBoolValue("IsPlayerInRange", false);
            CreateBoolValue("IsPlayerAttacking", false);
            CreateBoolValue("CanAttack", true);
            CreateBoolValue("IsAttacking", false);
            
            CreateFloatValue("DistanceToPlayer", 200.0f);
            CreateFloatValue("Health", 100.0f);
            CreateFloatValue("Stamina", 50.0f);
            
            CreateIntValue("AttackPower", 10);
            
            CreateStringValue("CurrentState", "Idle");
        }
    };
    
### Creating Actions
- Create a struct derived from the **ParamsForAction** base struct. This struct is used to define action parameters and is passed to the action class constructor.
- Create a class derived from the **HActionNode** base class.
- After creating your **action**, to register it to the **editor**, go to `NodeEditorApp.cpp` and call `AddActionNodeToBuilder<YourActionClass, YourActionParameterStruct>(Its name)` inside the **OnStart** function:
    ```cpp
    AddActionNodeToBuilder<MeleeEnemyAttackAction, MeleeEnemyAttackActionParameters>("Melee Enemy Attack Action");
    ```
    ### HActionNode & ParamsForAction
  - **ParamsForAction** provides a **DrawImGui** function to expose action parameters directly in the editor. Just assign parameters with `DrawXValue` function inside `DrawImGui` function.
  - If you want to assign Blackboard key value as a parameter create varaible name `HBlackboardKeyValue` in the `DrawImGui` function call `DrawBlackboardIntKeySelector` function for visualize it. (as shown in the example above).
  - HActionNode provides helper functions:
    - GetOwner() – access the owning AI / actor
    - GetBlackboard() – access the assigned blackboard
  ```cpp
  struct MeleeEnemyAttackActionParameters : ParamsForAction
  {
      HBlackboardKeyValue AttackPowerKey;
      float AttackDuration = 10.0f;
      void DrawImGui(HBlackboard* blackboard) override
      {
          DrawBlackboardIntKeySelector("Attack Power", AttackPowerKey, blackboard);
          DrawFloatValue("Attack Duration", AttackDuration);
      }
  };
  class MeleeEnemyAttackAction : public HActionNode
  {
  public:
      MeleeEnemyAttackAction(const std::string& name, const MeleeEnemyAttackActionParameters& params = MeleeEnemyAttackActionParameters{})
          : HActionNode(name, params), m_AttackPowerKey(params.AttackPowerKey), m_AttackDuration(params.AttackDuration) {}
  
      void OnStart() override;
      NodeStatus Update() override;
      void OnFinished() override;
      void OnAbort() override;
  private:
      HBlackboardKeyValue m_AttackPowerKey;
      float m_AttackDuration;
      float m_ElapsedTime = 0.0f;
  };
  ```
  

### Creating Conditions
- Create a struct derived from the **ParamsForCondition** base struct. This struct is used to define action parameters and is passed to the action class constructor.
- Create a class derived from the **HCondition** base class.
- After creating your **condition**, to register it to the **editor**, go to `NodeEditorApp.cpp` and call `AddConditionNodeToBuilder<YourConditionClass, YourConditionParameterStruct>(Its name)` inside the **OnStart** function:
    ```cpp
    AddConditionNodeToBuilder<IsPlayerInRangeCondition, IsPlayerInRangeParameters>("Is Player In Range Condition");
    ```
    ### HCondition & ParamsForCondition
    - **ParamsForCondition** provides a **DrawImGui** function to expose action parameters directly in the editor. Just assign parameters with `DrawXValue` function inside `DrawImGui` function.
    - If you want to assign Blackboard key value as a parameter create varaible name `HBlackboardKeyValue` in the `DrawImGui` function call `DrawBlackboardIntKeySelector` function for visualize it. (as shown in the example above).
    - HCondition provides helper functions:
        - GetOwner() – access the owning AI / actor
        - GetBlackboard() – access the assigned blackboard
        - GetPriorityMode() - returns the condition’s priority mode
        - GetLastStatus() - returns the last evaluated status (Success / Failure)
    ```cpp
    struct IsPlayerInRangeParameters : ParamsForCondition
    {
        float Range = 100.0f;
        HBlackboardKeyValue DistanceToPlayerKey;
        void DrawImGui(HBlackboard* blackboard) override
        {
            DrawFloatValue("Range", Range);
            DrawBlackboardFloatKeySelector("Distance To Player", DistanceToPlayerKey, blackboard);
        }
    };
    class IsPlayerInRangeCondition : public HCondition
    {
    public:
        IsPlayerInRangeCondition(const std::string& name, const IsPlayerInRangeParameters& params = IsPlayerInRangeParameters{})
            : HCondition(name, params), m_Range(params.Range), m_DistanceToPlayer(params.DistanceToPlayerKey) {}

        void OnStart() override;
        bool CheckCondition() override;
        void OnFinished() override;
        void OnAbort() override;
    private:
        float m_Range;
        HBlackboardKeyValue m_DistanceToPlayer;
    };
    ```

### Creating Decorators
- Create a struct derived from the **ParamsForDecorator** base struct. This struct is used to define action parameters and is passed to the action class constructor.
- Create a class derived from the **HDecorator** base class.
- After creating your **decorator**, to register it to the **editor**, go to `NodeEditorApp.cpp` and call `AddDecoratorNodeToBuilder<YourDecoratorClass, YourDecoratorParameterStruct>(Its name)` inside the **OnStart** function:
    ```cpp
    AddDecoratorNodeToBuilder<ChangeResultOfTheNodeDecorator, ChangeResultOfTheNodeParameters>("Change Result Of The Node Decorator");
    ```
    ```cpp
    struct ChangeResultOfTheNodeParameters : ParamsForDecorator
    {
        NodeStatus NewResult = NodeStatus::SUCCESS;
        void DrawImGui(HBlackboard* blackboard) override
        {
            const char* items[] = { "SUCCESS", "FAILURE", "RUNNING" };
            int currentItem = static_cast<int>(NewResult);
            if (ImGui::Combo("New Result", &currentItem, items, IM_ARRAYSIZE(items)))
            {
                NewResult = static_cast<NodeStatus>(currentItem);
            }
        }
    };
    class ChangeResultOfTheNodeDecorator : public HDecorator
    {
    public:
        ChangeResultOfTheNodeDecorator(const std::string& name, const ChangeResultOfTheNodeParameters& params = ChangeResultOfTheNodeParameters{})
            : HDecorator(name, params), m_NewResult(params.NewResult) {}
        void OnStart() override;
        bool CanExecute() override;
        void OnFinishedResult(NodeStatus& status) override;
        void OnFinished() override;
        void OnAbort() override;
    private:
        NodeStatus m_NewResult;
    };
    ```
    ### HDecorator & ParamsForDecorator
    - **ParamsForDecorator** provides a **DrawImGui** function to expose action parameters directly in the editor. Just assign parameters with `DrawXValue` function inside `DrawImGui` function.
    - If you want to assign Blackboard key value as a parameter create varaible name `HBlackboardKeyValue` in the `DrawImGui` function call `DrawBlackboardIntKeySelector` function for visualize it. (as shown in the example above).
    - HDecorator provides helper functions:
        - GetOwner() – access the owning AI / actor
        - GetBlackboard() – access the assigned blackboard

Script Example(Behavior Tree and Builder)

```cpp
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Nodes.h"
#include "BlackboardBase.h"
#include "Root.h"

class EnemyAI;

class BehaviorTree
{
public:
    BehaviorTree(EnemyAI* owner) : m_Owner(owner), m_Blackboard(nullptr) {}
    ~BehaviorTree();

    void StartTree();
    void TickTree();
    void StopTree();
    
    void SetRootNode(std::unique_ptr<HNode> root) { m_RootNode = std::move(root); }
    const HNode* GetRootNode() const { return m_RootNode.get(); }

private:
    bool m_bOwnsBlackboard = false;
    bool m_bIsRunning = false;
    EnemyAI* m_Owner;
    std::unique_ptr<HNode> m_RootNode;
    HBlackboard* m_Blackboard;

    friend class BehaviorTreeBuilder;
};

class BehaviorTreeBuilder
{
public:
    BehaviorTreeBuilder(EnemyAI* owner) : m_Tree(Root::CreateBehaviorTree(owner)) {}

    template<typename BlackboardType>
    BehaviorTreeBuilder& setBlackboard()
    {
        static_assert(std::is_base_of_v<HBlackboard, BlackboardType>, "BlackboardType must derive from HBlackboard");
        auto blackboard = new BlackboardType();
        m_Tree->m_Blackboard = blackboard;
        m_Tree->m_bOwnsBlackboard = true;
        return *this;
    }
    BehaviorTreeBuilder& setBlackboard(HBlackboard* blackboard)
    {
        m_Tree->m_Blackboard = blackboard;
        m_Tree->m_bOwnsBlackboard = false;
        return *this;
    }
    BehaviorTreeBuilder& root(NodeEditorApp* editorApp);
    BehaviorTreeBuilder& sequence(const std::string& name);
    BehaviorTreeBuilder& selector(const std::string& name);
    template<typename ActionNodeType, typename... Args>
    BehaviorTreeBuilder& action(Args&&... args)
    {
        static_assert(std::is_base_of_v<HActionNode, ActionNodeType>, "ActionNodeType must derive from HAction");
        auto action = std::make_unique<ActionNodeType>(std::forward<Args>(args)...);
        std::cout << "Adding Action Node: " << action->GetName() << std::endl;
        m_LastCreatedNode = action.get();
        if (m_CurrentDecorator)
        {
            auto decoratorNode = std::move(m_CurrentDecorator);
            auto decoratorNodePtr = decoratorNode.get();
            if (!m_NodeStack.empty())
            {
                action->SetOwner(m_Tree->m_Owner);
                action->SetType(HNodeType::Action);
                action->SetBlackboard(m_Tree->m_Blackboard);
                decoratorNode->AddChild(std::move(action));
                m_NodeStack.back()->AddChild(std::move(decoratorNode));
                auto editorApp = decoratorNodePtr->GetParent()->GetEditorApp();
                if (editorApp == nullptr)
                    std::cout << "editor app null" << std::endl;
                m_LastCreatedNode->SetEditorApp(editorApp);
            }
        }
        else
            if (!m_NodeStack.empty())
            {
                action->SetOwner(m_Tree->m_Owner);
                action->SetType(HNodeType::Action);
                action->SetBlackboard(m_Tree->m_Blackboard);
                m_NodeStack.back()->AddChild(std::move(action));
                auto editorApp = m_LastCreatedNode->GetParent()->GetEditorApp();
                if (editorApp == nullptr)
                    std::cout << "editor app null" << std::endl;
                m_LastCreatedNode->SetEditorApp(editorApp);
            }
        return *this;
    }
    template<typename ConditionNodeType, typename... Args>
    BehaviorTreeBuilder& condition(PriorityType priority, Args&&... args)
    {
        static_assert(std::is_base_of_v<HCondition, ConditionNodeType>, "ConditionNodeType must derive from HCondition");
        auto condition = std::make_unique<ConditionNodeType>(std::forward<Args>(args)...);
        std::cout << "Adding Condition Node: " << condition->GetName() << std::endl;
        if (m_LastCreatedNode)
        {
            condition->SetOwner(m_Tree->m_Owner);
            condition->SetBlackboard(m_Tree->m_Blackboard);
            condition->SetPriorityMode(priority);
            condition->SetType(HNodeType::Condition);
            m_LastCreatedNode->AddConditionNode(std::move(condition));
        }
        return *this;
    }
    template<typename DecoratorNodeType, typename... Args>
    BehaviorTreeBuilder& decorator(Args&&... args)
    {
        static_assert(std::is_base_of_v<HDecorator, DecoratorNodeType>, "DecoratorNodeType must derive from HDecorator");
        m_CurrentDecorator = std::make_unique<DecoratorNodeType>(std::forward<Args>(args)...);
        std::cout << "Adding Decorator Node: " << m_CurrentDecorator->GetName() << std::endl;
        m_CurrentDecorator->SetOwner(m_Tree->m_Owner);
        m_CurrentDecorator->SetBlackboard(m_Tree->m_Blackboard);
        m_CurrentDecorator->SetType(HNodeType::Decorator);
        return *this;
    }
    BehaviorTreeBuilder& end();
    BehaviorTree* build() const;

    const HNode* GetLastCreatedNode() const { return m_LastCreatedNode; }
private:
    BehaviorTree* m_Tree;
    HNode* m_LastCreatedNode = nullptr;
    std::unique_ptr<HDecorator> m_CurrentDecorator;
    std::vector<HNode*> m_NodeStack;
};

#include "Tree.h"
#include "CompositeNodes.h"

//BehaviorTree methods
BehaviorTree* BehaviorTreeBuilder::build() const 
{
    std::cout << "Behavior Tree Built" << std::endl;
    return m_Tree;
}

BehaviorTree::~BehaviorTree()
{
    m_Owner = nullptr;
    if (m_bOwnsBlackboard)
        delete m_Blackboard;
    m_Blackboard = nullptr;
}

void BehaviorTree::StartTree()
{
    std::cout << "Behavior Tree Started" << std::endl;
    m_bIsRunning = true;
}

void BehaviorTree::TickTree()
{
    if (m_RootNode && m_bIsRunning && m_Blackboard)
    {
        m_RootNode->Tick();
        m_Blackboard->ClearValuesChangedFlag();
    }
}

void BehaviorTree::StopTree()
{
    std::cout << "Behavior Tree Stopped" << std::endl;
    if (!m_bIsRunning)
        return;
    m_bIsRunning = false;
    if (m_RootNode)
        m_RootNode->OnAbort();
}

// BehaviorTreeBuilder methods

BehaviorTreeBuilder& BehaviorTreeBuilder::root(NodeEditorApp* editorApp)
{
    std::cout << "Adding Root Node" << std::endl;
    auto rootNode = std::make_unique<HRootNode>();
    rootNode->SetEditorApp(editorApp);
    HRootNode* rootNodePtr = rootNode.get();
    rootNodePtr->SetType(HNodeType::Root);
    m_LastCreatedNode = rootNodePtr;
    m_Tree->SetRootNode(std::move(rootNode));
    m_NodeStack.push_back(rootNodePtr);
    return *this;
}

BehaviorTreeBuilder& BehaviorTreeBuilder::sequence(const std::string& name)
{
    std::cout << "Adding Sequence Node: " << name << std::endl;
    auto sequenceNode = std::make_unique<SequenceNode>(name);
    SequenceNode* sequenceNodePtr = sequenceNode.get();
    sequenceNodePtr->SetType(HNodeType::Composite);
    m_LastCreatedNode = sequenceNodePtr;
    if (m_CurrentDecorator)
    {
        auto decoratorNode = std::move(m_CurrentDecorator);
        auto decoratorNodePtr = decoratorNode.get();
        decoratorNode->AddChild(std::move(sequenceNode));
        m_NodeStack.back()->AddChild(std::move(decoratorNode));
        auto editorApp = decoratorNodePtr->GetParent()->GetEditorApp();
        if (editorApp == nullptr)
            std::cout << "editor app null" << std::endl;
        sequenceNodePtr->SetEditorApp(editorApp);
        m_NodeStack.push_back(sequenceNodePtr);
    }
    else
    {
        m_NodeStack.back()->AddChild(std::move(sequenceNode));
        m_NodeStack.push_back(sequenceNodePtr);
        auto editorApp = sequenceNodePtr->GetParent()->GetEditorApp();
        if (editorApp == nullptr)
            std::cout << "editor app null" << std::endl;
        sequenceNodePtr->SetEditorApp(editorApp);
    }
    return *this;
}

BehaviorTreeBuilder& BehaviorTreeBuilder::selector(const std::string& name)
{
    std::cout << "Adding Selector Node: " << name << std::endl;
    auto selectorNode = std::make_unique<SelectorNode>(name);
    auto selectorNodePtr = selectorNode.get();
    selectorNodePtr->SetType(HNodeType::Composite);
    m_LastCreatedNode = selectorNodePtr;
    if (m_CurrentDecorator)
    {
        auto decoratorNode = std::move(m_CurrentDecorator);
        auto decoratorNodePtr = decoratorNode.get();
        decoratorNode->AddChild(std::move(selectorNode));
        m_NodeStack.back()->AddChild(std::move(decoratorNode));
        auto editorApp = decoratorNodePtr->GetParent()->GetEditorApp();
        if (editorApp == nullptr)
            std::cout << "editor app null" << std::endl;
        selectorNodePtr->SetEditorApp(editorApp);
        m_NodeStack.push_back(selectorNodePtr);
    }
    else
    {
        m_NodeStack.back()->AddChild(std::move(selectorNode));
        m_NodeStack.push_back(selectorNodePtr);
        auto editorApp = selectorNodePtr->GetParent()->GetEditorApp();
        if (editorApp == nullptr)
            std::cout << "editor app null" << std::endl;
        selectorNodePtr->SetEditorApp(editorApp);
    }
    return *this;
}

BehaviorTreeBuilder& BehaviorTreeBuilder::end()
{
    std::cout << "Ending Current Node" << std::endl;
    if (!m_NodeStack.empty())
        m_NodeStack.pop_back();
    return *this;
}

```

## Future Goals
- Adding a simple Parallel Node
- Service node
- Subtree
- Supporting multiple behavior trees in a single editor
- Save / Load system for behavior trees
