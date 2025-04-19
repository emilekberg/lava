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

        inline ecs::SceneArchetype& getActiveScene()
        {
            return _scene;
        }
        App& addEcsSystem(std::function<void(lava::ecs::SceneArchetype&)> system)
        {
            _systems.push_back(system);
            return *this;
        }

    private:
        void setFrameBufferResized();
        
        std::unique_ptr<core::Window> _window;
        static void handleWindowResize(GLFWwindow* window, int width, int height);
        rendering::VulkanRenderer _vulkanRenderer;
        std::vector<std::function<void(lava::ecs::SceneArchetype&)>> _systems; 
        ecs::SceneArchetype _scene;
    };

}