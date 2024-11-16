#include "lava/rendering/data/texture.hpp"

namespace lava::rendering::data
{
    Texture::Texture(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, int width, int height, vk::Format format)
        : _width(width), _height(height), _image(VK_NULL_HANDLE), _memory(VK_NULL_HANDLE), _format(format)
    {
        vk::ImageCreateInfo imageInfo{};
        imageInfo
            .setImageType(vk::ImageType::e2D)
            .setExtent({static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1})
            .setMipLevels(1)
            .setArrayLayers(1)
            .setFormat(format)
            .setTiling(vk::ImageTiling::eOptimal)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSamples(vk::SampleCountFlagBits::e1);

        _image = vk::raii::Image(device, imageInfo);
        auto memoryRequirements = _image.getMemoryRequirements();

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo
            .setAllocationSize(memoryRequirements.size)
            .setMemoryTypeIndex(rendering::Buffer::findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));

        _memory = vk::raii::DeviceMemory(std::move(device.allocateMemory(allocInfo)));
        _image.bindMemory(_memory, 0);
 
    }
    Texture::~Texture()
    {
    }

}