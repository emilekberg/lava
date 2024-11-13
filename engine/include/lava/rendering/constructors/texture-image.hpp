#pragma once
#include <vulkan/vulkan_raii.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb-img.h"
#include "lava/rendering/constructors/buffer.hpp"
namespace lava::rendering::constructors
{
    std::unique_ptr<vk::raii::Image> createImage(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties)
    {
        vk::ImageCreateInfo imageInfo{};
        imageInfo
            .setImageType(vk::ImageType::e2D)
            .setExtent({width, height, 1})
            .setMipLevels(1)
            .setArrayLayers(1)
            .setFormat(format)
            .setTiling(tiling)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setUsage(usage)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSamples(vk::SampleCountFlagBits::e1);

        std::unique_ptr<vk::raii::Image> textureImage = std::make_unique<vk::raii::Image>(device, imageInfo);
        
        auto memoryRequirements = textureImage->getMemoryRequirements();

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo
            .setAllocationSize(memoryRequirements.size)
            .setMemoryTypeIndex(findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties));

        std::unique_ptr<vk::raii::DeviceMemory> textureMemory = std::make_unique<vk::raii::DeviceMemory>(std::move(device.allocateMemory(allocInfo)));
        textureImage->bindMemory(*textureMemory.get(), 0);

        return textureImage;
    }
    std::unique_ptr<vk::raii::Image> createTextureImage(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice)
    {
        int textureWidth, textureHeight, textureChannels;
        stbi_uc* pixels = stbi_load("./build/mesh/viking_room.png", &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);

        vk::DeviceSize imageSize = textureWidth * textureHeight * 4;

        if(!pixels) {
            throw std::runtime_error("failed to load image texture");
        }

        std::unique_ptr<vk::raii::Buffer> stagingBuffer;
        std::unique_ptr<vk::raii::DeviceMemory> stagingBufferMemory;

        std::tie(stagingBuffer, stagingBufferMemory) = createBuffer(device, physicalDevice, imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        void* data = stagingBufferMemory->mapMemory(0, imageSize);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        stagingBufferMemory->unmapMemory();
        stbi_image_free(pixels);

        std::unique_ptr<vk::raii::Image> image = createImage(device, physicalDevice, textureWidth, textureHeight, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal);



        return std::move(image);
    
    }
}