#include "lava/app.hpp"
#include <set>
// #include <imgui.h>
#define UNHANDLED_PARAMETER(param) param;
#undef max
namespace lava
{
    App::App() : 
        _window(std::make_unique<core::Window>()), 
        _vulkanRenderer(_window->getScreenSize(), _window->getWindowHandle())
    {
        glfwSetWindowUserPointer(_window->getGLFWwindow(), this);
        _window->setResizeHandler(handleWindowResize);
        _window->activate();
        _meshes = 
        {
            rendering::data::Mesh()
        };

    }
    App::~App()
    {
    }

    void App::setFrameBufferResized()
    {
        _vulkanRenderer.requireResize();
    }

    void App::handleWindowResize(GLFWwindow *window, int width, int height)
    {
        auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
        app->setFrameBufferResized();
    }

    void App::run()
    {
        bool requiresResize = false;
        while (!_window->shouldClose())
        {
            update();
            if(requiresResize)
            {
                rendering::ScreenSize screenSize = _window->getScreenSize();
                // pauses while screen is minimized.
                // add sleep?
                while(screenSize.width == 0 || screenSize.height == 0)
                {
                    _window->pollEvents();
                    screenSize = _window->getScreenSize();
                }
                _vulkanRenderer.resize(screenSize);
            }
            _window->pollEvents();
            requiresResize = render();
        }
        _vulkanRenderer.waitUntilIdle();
    }

    void App::update()
    {
    }

    bool App::render()
    {
        bool requiresResize = false;

        requiresResize = _vulkanRenderer.render();
        return requiresResize;
    }
}
