#include "lava/app.h"

namespace lava
{
    App::App()
        : _window(std::make_unique<core::Window>())
    {
        _window->activate();
    }
    App::~App()
    {
        _window = nullptr;
    }
    void App::run()
    {
        while(!_window->shouldClose())
        {
            _window->pollEvents();
            update();
            render();
        }
    }

    void App::update()
    {

    }

    void App::render()
    {		
    
    }
}