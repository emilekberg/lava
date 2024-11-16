#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <functional>
namespace lava::rendering
{
    class Buffer
    {
    public:
        Buffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
        ~Buffer();

        vk::raii::Buffer& getVkBuffer();
        vk::raii::DeviceMemory& getVkDeviceMemory();

        void mapMemory(vk::DeviceSize offset, vk::DeviceSize size, std::function<void(void*)> callback);
        void* mapMemory(vk::DeviceSize offset, vk::DeviceSize size);
        void unmapMemory();

        static uint32_t findMemoryType(const vk::raii::PhysicalDevice& physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    private:
        vk::raii::Buffer _vkBuffer;
        vk::raii::DeviceMemory _vkDeviceMemory; 
    };
}