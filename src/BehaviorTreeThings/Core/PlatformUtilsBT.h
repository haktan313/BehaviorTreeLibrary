#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class PlatformUtilsBT
{
public:
    static std::string OpenFile(const char* filter);
    static std::string SaveFile(const char* filter);
};
