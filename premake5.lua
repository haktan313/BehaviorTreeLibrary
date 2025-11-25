
workspace "BehaviorTreeProject"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "BehaviorTreeProject"

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
        "libs/imgui-1.92.4/*.cpp",
        "libs/imgui-1.92.4/backends/imgui_impl_glfw.cpp",
        "libs/imgui-1.92.4/backends/imgui_impl_opengl3.cpp",

        "libs/imgui-node-editor/crude_json.cpp",
        "libs/imgui-node-editor/imgui_canvas.cpp",
        "libs/imgui-node-editor/imgui_node_editor.cpp",
        "libs/imgui-node-editor/imgui_node_editor_api.cpp"
    }
 
    includedirs {
        "src",
        "src/BehaviorTreeThings",
        "src/BehaviorTreeThings/Core",
        "src/BehaviorTreeThings/CustomThings",
        "libs/glad/include",
        "libs/glfw-3.4.bin.WIN64/include",
        "libs/imgui-1.92.4",
        "libs/imgui-1.92.4/backends",
        "libs/imgui-node-editor"
    }

    libdirs {
        "libs/glfw-3.4.bin.WIN64/lib-vc2022"
    }

    links {
        "glfw3",
        "opengl32"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"