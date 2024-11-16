#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "lava/rendering/buffer.hpp"
namespace lava::rendering::data
{
    class Texture
    {
    public:
        Texture(const vk::raii::Device &device, const vk::raii::PhysicalDevice& physicalDevice, rendering::Buffer &stagingBuffer, int width, int height);
        ~Texture();

        const vk::raii::Image& getVkImage() const
        {
            return _image;
        }
        const vk::raii::DeviceMemory& getVkMemory() const
        {
            return _memory;
        }

        int getWidth()
        {
            return _width;
        }
        int getHeight()
        {
            return _height;
        }

    private:
        vk::raii::Image _image;
        vk::raii::DeviceMemory _memory;
        int _width, _height;
    };
}