#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "lava/rendering/buffer.hpp"
namespace lava::rendering::data
{
    class Texture
    {
    public:
        Texture(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, int width, int height, vk::Format format);
        Texture(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, int width, int height, vk::Format format, vk::ImageUsageFlags usage);
        ~Texture();

        const vk::raii::Image &getVkImage() const
        {
            return _image;
        }
        const vk::raii::DeviceMemory &getVkMemory() const
        {
            return _memory;
        }

        int getWidth() const
        {
            return _width;
        }
        int getHeight() const
        {
            return _height;
        }
        const vk::Format getFormat() const
        {
            return _format;
        }

    private:
        vk::raii::Image _image;
        vk::raii::DeviceMemory _memory;
        vk::Format _format;
        int _width, _height;
    };
}