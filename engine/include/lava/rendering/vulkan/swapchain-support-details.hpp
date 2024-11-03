#pragma once
#include <vulkan/vulkan.h>
#include <vector>
namespace lava::rendering::vulkan
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
    SwapChainSupportDetails querySwapChainSupport(const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR &surface)
    {
        SwapChainSupportDetails details;
        details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
        details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
        details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
        return details;
    }
}