#pragma once
#define VK_USE_PLATFORM_WIN31_KHR
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN31
#include <GLFW/glfw3native.h>
#include <tuple>
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

        std::tuple<int, int> getSize() const;

        HWND getWindowHandle();
        GLFWwindow* getGLFWwindow();

        template <typename T>
        void setOwner(T* owner);
        void setResizeHandler(void(callback)(GLFWwindow* window, int width, int height));

    private:
        GLFWwindow* _glfwWindow;
    };
}