#include "lava/rendering/buffer.hpp"

namespace lava::rendering
{
    Buffer::Buffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) :
        _vkBuffer(VK_NULL_HANDLE),
        _vkDeviceMemory(VK_NULL_HANDLE)
    {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(size);
        bufferInfo.setUsage(usage);
        bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

        _vkBuffer = vk::raii::Buffer(device, bufferInfo);

        auto memoryRequirements = _vkBuffer.getMemoryRequirements();

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.setAllocationSize(memoryRequirements.size);
        allocInfo.setMemoryTypeIndex(findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties));

        _vkDeviceMemory = vk::raii::DeviceMemory(device, allocInfo);
        _vkBuffer.bindMemory(_vkDeviceMemory, 0);
    }
    Buffer::~Buffer()
    {

    }

    vk::raii::Buffer& Buffer::getVkBuffer()
    {
        return _vkBuffer;
    }
    
    vk::raii::DeviceMemory& Buffer::getVkDeviceMemory()
    {
        return _vkDeviceMemory;
    }
    void Buffer::mapMemory(vk::DeviceSize offset, vk::DeviceSize size, std::function<void(void*)> callback)
    {
        void* mappedMemory = _vkDeviceMemory.mapMemory(offset, size);
        callback(mappedMemory);
        _vkDeviceMemory.unmapMemory();
    }

    uint32_t Buffer::findMemoryType(const vk::raii::PhysicalDevice& physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
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
}