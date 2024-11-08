#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <memory>
#include "lava/rendering/queue-family.hpp"
namespace lava::rendering::constructors
{
    std::unique_ptr<vk::raii::CommandPool> createCommandPool(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface)
    {
        rendering::QueueFamilyIndices queueFamilyIndices = rendering::findQueueFamilies(physicalDevice, surface);

        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        poolInfo.setQueueFamilyIndex(queueFamilyIndices.graphicsFamily.value());

        return std::make_unique<vk::raii::CommandPool>(device, poolInfo);
    }
}