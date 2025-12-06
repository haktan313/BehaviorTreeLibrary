#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EnemyAI.h"
#include "Editor/NodeEditor.h"
#include "Editor/NodeEditorApp.h"

class App
{
public:
    App();
    ~App();
    
    void Run();
    bool Init();
    void Shutdown();

    static void SizeCallback(GLFWwindow* window, int width, int height);

    static App* Get() { return s_Instance; }
private:
    EnemyAI* m_EnemyAI;
    GLFWwindow* m_Window;
    ImGuiContext* m_ImGuiContext;
    
    std::unique_ptr<NodeEditorApp> m_NodeEditorApp;
    
    static App* s_Instance;
};
