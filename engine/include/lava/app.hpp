#pragma once

#include <vector>
#include <memory>
#include <optional>
#include "lava/core/window.hpp"
#include "lava/rendering/vulkan-renderer.hpp"
#include "lava/rendering/data/mesh.hpp"
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

    private:
        void setFrameBufferResized();
        
        std::unique_ptr<core::Window> _window;
        static void handleWindowResize(GLFWwindow* window, int width, int height);
        rendering::VulkanRenderer _vulkanRenderer;


        std::vector<rendering::data::Mesh> _meshes;
    };

}