#include "Tree.h"
#include "CompositeNodes.h"

//BehaviorTree methods
BehaviorTree* BehaviorTreeBuilder::build()
{
    std::cout << "Behavior Tree Built" << std::endl;
    return m_Tree;
}

BehaviorTree::~BehaviorTree()
{
    m_Owner = nullptr;
    m_Blackboard = nullptr;
}

void BehaviorTree::StartTree()
{
    std::cout << "Behavior Tree Started" << std::endl;
    m_bIsRunning = true;
}

void BehaviorTree::TickTree()
{
    if (m_RootNode && m_bIsRunning)
        m_RootNode->Tick();
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
    m_LastCreatedNode = sequenceNodePtr;
    if (m_CurrentDecorator)
    {
        auto decoratorNode = std::move(m_CurrentDecorator);
        decoratorNode->AddChild(std::move(sequenceNode));
        if (m_NodeStack.empty())
            m_Tree->SetRootNode(std::move(decoratorNode));
        else
            m_NodeStack.back()->AddChild(std::move(decoratorNode));
    }
    else
    {
        if (m_NodeStack.empty())
            m_Tree->SetRootNode(std::move(sequenceNode));
        else
            m_NodeStack.back()->AddChild(std::move(sequenceNode));
    }
    m_NodeStack.push_back(sequenceNodePtr);
    sequenceNodePtr->SetEditorApp(sequenceNodePtr->GetParent()->GetEditorApp());
    return *this;
}

BehaviorTreeBuilder& BehaviorTreeBuilder::selector(const std::string& name)
{
    std::cout << "Adding Selector Node: " << name << std::endl;
    auto selectorNode = std::make_unique<SelectorNode>(name);
    auto selectorNodePtr = selectorNode.get();
    m_LastCreatedNode = selectorNodePtr;
    if (m_CurrentDecorator)
    {
        auto decoratorNode = std::move(m_CurrentDecorator);
        decoratorNode->AddChild(std::move(selectorNode));
        if (m_NodeStack.empty())
            m_Tree->SetRootNode(std::move(decoratorNode));
        else
            m_NodeStack.back()->AddChild(std::move(decoratorNode));
        
        m_NodeStack.push_back(selectorNodePtr);
    }
    else
    {
        if (m_NodeStack.empty())
            m_Tree->SetRootNode(std::move(selectorNode));
        else
            m_NodeStack.back()->AddChild(std::move(selectorNode));
        m_NodeStack.push_back(selectorNodePtr);
    }
    selectorNodePtr->SetEditorApp(selectorNodePtr->GetParent()->GetEditorApp());
    return *this;
}

BehaviorTreeBuilder& BehaviorTreeBuilder::end()
{
    std::cout << "Ending Current Node" << std::endl;
    if (!m_NodeStack.empty())
        m_NodeStack.pop_back();
    return *this;
}
