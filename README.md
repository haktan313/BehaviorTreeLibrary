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


## Future Goals
- Adding a simple Parallel Node
- Service node
- Subtree
- Supporting multiple behavior trees in a single editor
- Save / Load system for behavior trees
