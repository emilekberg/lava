#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <tuple>
#include <memory>
namespace lava::rendering::constructors
{
    uint32_t findMemoryType(const vk::raii::PhysicalDevice& physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    std::tuple<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
}