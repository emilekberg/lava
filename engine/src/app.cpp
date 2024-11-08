#include "lava/app.hpp"
#include <set>

#define UNHANDLED_PARAMETER(param) param;
#undef max
namespace lava
{
    App::App()
        : _window(std::make_unique<core::Window>()), _vulkanRenderer(*_window.get())
    {
        glfwSetWindowUserPointer(_window->getGLFWwindow(), this);
        _window->setResizeHandler(handleWindowResize);
        _window->activate();
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
            if(requiresResize)
            {
                rendering::ScreenSize screenSize = _window->getScreenSize();
                while(screenSize.width == 0 || screenSize.height == 0)
                {
                    _window->pollEvents();
                    screenSize = _window->getScreenSize();
                }
                _vulkanRenderer.resize(screenSize);
                // do resize stuff
            }
            _window->pollEvents();
            update();
            requiresResize = _vulkanRenderer.render();
        }
        _vulkanRenderer.waitUntilIdle();
    }

    void App::update()
    {
    }
}
