#pragma once
#define VK_USE_PLATFORM_WIN31_KHR
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN31
#include <GLFW/glfw3native.h>

namespace lava::core
{
    class Window
    {
    public:
        Window();
        ~Window();

        void close();
        bool shouldClose();
        void pollEvents();
        void activate();

        HWND getWindowHandle();
        GLFWwindow* getGLFWwindow();

    private:
        GLFWwindow* _glfwWindow;
    };
}