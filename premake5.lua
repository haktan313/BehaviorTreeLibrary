
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
        "src/**.cpp"
    }
 
    includedirs {
           "src",
           "src/BehaviorTreeThings",
           "src/BehaviorTreeThings/Core",
           "src/BehaviorTreeThings/CustomThings"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"