#pragma once
#include <glfw/glfw3.h>
#include <lava/core/window.h>
#include <memory>
namespace lava
{
    class App
    {
    public:
        App();
        ~App();
        void run();
        void update();
        void render();

    private:
        std::unique_ptr<core::Window> _window;
    };
}