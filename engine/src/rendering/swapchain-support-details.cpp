#pragma once
#include "lava/rendering/swapchain-support-details.hpp"
namespace lava::rendering
{

    SwapChainSupportDetails querySwapChainSupport(const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR &surface)
    {
        SwapChainSupportDetails details;
        details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
        details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
        details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
        return details;
    }
}
