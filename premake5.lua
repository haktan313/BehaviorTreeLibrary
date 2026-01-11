
workspace "BehaviorTreeProject"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "BehaviorTreeProject"

    project "yaml-cpp"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("bin/%{cfg.buildcfg}/%{prj.name}")
    objdir    ("bin-int/%{cfg.buildcfg}/%{prj.name}")

    files
    {
        "libs/yaml-cpp/src/**.cpp",
        "libs/yaml-cpp/src/**.h",
        "libs/yaml-cpp/include/**.h",
        "libs/yaml-cpp/include/**.hpp"
    }

    includedirs
    {
        "libs/yaml-cpp/include"
    }

    defines
    {
        "YAML_CPP_STATIC_DEFINE"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"


project "BehaviorTreeProject"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
     targetdir ("bin/%{cfg.buildcfg}")
     objdir ("bin-int/%{cfg.buildcfg}")
     
    files {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp",
        "libs/glad/src/glad.c", 
        
        "libs/imgui/imgui.cpp",
        "libs/imgui/imgui.h",
        "libs/imgui/imgui_draw.cpp",
        "libs/imgui/imgui_widgets.cpp",
        "libs/imgui/imgui_tables.cpp",
        "libs/imgui/imgui_demo.cpp",
        
        "libs/imgui/backends/imgui_impl_glfw.cpp",
        "libs/imgui/backends/imgui_impl_opengl3.cpp",

        "libs/imgui-node-editor/crude_json.cpp",
        "libs/imgui-node-editor/imgui_canvas.cpp",
        "libs/imgui-node-editor/imgui_node_editor.cpp",
        "libs/imgui-node-editor/imgui_node_editor_api.cpp",

        "libs/yaml-cpp/include/**.h"        
    }
 
    includedirs {
        "src",
        "src/BehaviorTreeThings",
        "src/BehaviorTreeThings/Core",
        "src/BehaviorTreeThings/CustomThings",
        "libs/glad/include",
        "libs/glfw-3.4.bin.WIN64/include",
        "libs/imgui",
        "libs/imgui/backends",
        "libs/imgui-node-editor",
        "libs/yaml-cpp/include"
    }

    libdirs {
        "libs/glfw-3.4.bin.WIN64/lib-vc2022"
    }

    links {
        "glfw3",
        "opengl32",
        "yaml-cpp"
    }

    defines { "YAML_CPP_STATIC_DEFINE" }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"