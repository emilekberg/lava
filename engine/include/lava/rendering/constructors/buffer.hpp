#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <tuple>
#include <memory>
namespace lava::rendering::constructors
{
    uint32_t findMemoryType(const vk::raii::PhysicalDevice& physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        throw std::runtime_error("failed to find suitable memory type!");
    }
    std::tuple<std::unique_ptr<vk::raii::Buffer>, std::unique_ptr<vk::raii::DeviceMemory>> createBuffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
    {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(size);
        bufferInfo.setUsage(usage);
        bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

        vk::raii::Buffer buffer(device, bufferInfo);

        auto memoryRequirements = buffer.getMemoryRequirements();

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.setAllocationSize(memoryRequirements.size);
        allocInfo.setMemoryTypeIndex(findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties));

        vk::raii::DeviceMemory bufferMemory(device, allocInfo);
        buffer.bindMemory(bufferMemory, 0);

        return std::make_tuple(std::make_unique<vk::raii::Buffer>(std::move(buffer)), std::make_unique<vk::raii::DeviceMemory>(std::move(bufferMemory)));
    }
}