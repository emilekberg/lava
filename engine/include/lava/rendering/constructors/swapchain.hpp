#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <glfw/glfw3.h>
#include "lava/core/window.hpp"
#include "lava/rendering/screensize.hpp"
#include "lava/rendering/swapchain-support-details.hpp"
#include "lava/rendering/queue-family.hpp"
#undef max
namespace lava::rendering::constructors
{
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, const ScreenSize& screenSize);
    std::tuple<std::unique_ptr<vk::raii::SwapchainKHR>, vk::Format, vk::Extent2D> createSwapChain(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR &surface, const ScreenSize& screenSize);
    std::tuple<vk::raii::SwapchainKHR, vk::Format, vk::Extent2D> createSwapChain2(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR &surface, const ScreenSize& screenSize);
    std::tuple<std::unique_ptr<vk::raii::SwapchainKHR>, vk::Extent2D> recreateSwapChain(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR &surface, const ScreenSize& screenSize, const vk::raii::SwapchainKHR& oldSwapChain);

}