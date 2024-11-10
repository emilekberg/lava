#pragma once
#include <vulkan/vulkan_raii.hpp>
namespace lava::rendering
{
    class RenderContext
    {
        const vk::raii::Device& vkDevice;
        const vk::raii::SurfaceKHR& vkSurface;
        // const GLFWwindow* glfwWindow;
    };
}