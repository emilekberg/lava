#pragma once
#include <vulkan/vulkan_raii.hpp>
namespace lava::rendering
{
    class VulkanRenderer
    {
    public:
        VulkanRenderer();
        ~VulkanRenderer();
        
        void begin();
        void render();
        void end();

    private:
    };
}