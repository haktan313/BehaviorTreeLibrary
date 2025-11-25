#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EnemyAI.h"

class App
{
public:
    App();
    ~App();
    void Run();
    bool Init();
    void Shutdown();

    static void SizeCallback(GLFWwindow* window, int width, int height);

private:
    EnemyAI* m_EnemyAI;
    GLFWwindow* m_Window;
    
    static App* s_Instance;
};
