#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include "lava/core/window.hpp"
#include "lava/rendering/vulkan-renderer.hpp"
#include "lava/rendering/data/mesh.hpp"
#include "lava/ecs/scene-archetype.hpp"
#include "lava/ecs/component.hpp"
#include "lava/ecs/world.hpp"
namespace lava
{
    class App
    {
    public:
        App();
        ~App();
        void run();
        void update();
        bool render();

        void setWorld(std::unique_ptr<lava::ecs::World> world)
        {
            _world = std::move(world);
        }

    private:
        void setFrameBufferResized();
        
        std::unique_ptr<core::Window> _window;
        static void handleWindowResize(GLFWwindow* window, int width, int height);
        rendering::VulkanRenderer _vulkanRenderer;
        std::unique_ptr<lava::ecs::World> _world;
    };

}