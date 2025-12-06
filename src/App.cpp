#include "App.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "BehaviorTreeThings/Core/Tree.h"
#include "Editor/NodeEditorApp.h"

App* App::s_Instance = nullptr;

App::App() : m_EnemyAI(nullptr), m_Window(nullptr)
{
    s_Instance = this;
    m_NodeEditorApp = std::make_unique<NodeEditorApp>();
    Root::RootStart();
    m_EnemyAI = new EnemyAI();
    m_NodeEditorApp->SetEnemyAI(m_EnemyAI);
}

App::~App()
{
    Root::RootStop();
    delete m_EnemyAI;
    m_EnemyAI = nullptr;
}

void App::Run()
{
    if (!Init())
    {
        std::cout << "Application failed to initialize!" << std::endl;
        Shutdown();
        return;
    }
    ImGui::SetCurrentContext(m_ImGuiContext);
    while (m_EnemyAI != nullptr && !glfwWindowShouldClose(m_Window))
    {
        Root::RootTick();
        m_EnemyAI->Tick(0.016f);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiIO& io = ImGui::GetIO();
        
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(io.DisplaySize);

        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize   |
            ImGuiWindowFlags_NoMove     |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("MainWindow_Fullscreen", nullptr, windowFlags);
        ImGui::PopStyleVar();
        
        m_NodeEditorApp->Update();
        
        ImGui::End(); 
        ImGui::Render();

        int width, height;
        glfwGetFramebufferSize(m_Window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
    Shutdown();
}

bool App::Init()
{
    if (!glfwInit())
    {
        std::cout << "GLFW initialization failed!" << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(1280, 720, "NavMesh Example", NULL, NULL);
    if (m_Window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwSetWindowUserPointer(m_Window, this);
    glfwMakeContextCurrent(m_Window);
    glfwSetFramebufferSizeCallback(m_Window, App::SizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_ImGuiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_ImGuiContext);
    
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    m_NodeEditorApp->OnStart();
    return true;
}

void App::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_Window)
        glfwDestroyWindow(m_Window);
    m_Window = nullptr;
    
    glfwTerminate();
}

void App::SizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
