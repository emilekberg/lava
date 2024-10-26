#pragma once
#include <glfw/glfw3.h>
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


    private:
        GLFWwindow* _windowHandle;
    };
}