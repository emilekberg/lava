#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <vector>
namespace lava::rendering
{
    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
        bool isAdequate()
        {
            return !formats.empty() && !presentModes.empty();
        }
    };
    SwapChainSupportDetails querySwapChainSupport(const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR &surface);
 
}