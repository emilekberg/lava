#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "lava/rendering/buffer.hpp"
#include <memory>
#include <any>
namespace lava::rendering
{
    enum CommandType
    {
        NONE,
        COPY_IMAGE_TO_BUFFER,
        COPY_BUFFER,
        TRANSITION_IMAGE_LAYOUT,
    };

    struct Command
    {
        CommandType type;
        std::any data;
        
        template<class T>
        std::shared_ptr<T> getData()
        {
            return std::any_cast<std::shared_ptr<T>>(data);
        }
    };
    class CopyBufferToImageCommand
    {
    public:
        CopyBufferToImageCommand(std::unique_ptr<Buffer> sourceBuffer, const vk::raii::Image& image, uint32_t width, uint32_t height) 
        : sourceBuffer(std::move(sourceBuffer)), destinationImage(std::move(image)), width(width), height(height) 
        {}
        std::unique_ptr<Buffer> sourceBuffer;
        const vk::raii::Image& destinationImage;
        uint32_t width;
        uint32_t height;
    };
    class CopyBufferCommand
    {
    public:
        CopyBufferCommand(std::unique_ptr<Buffer> sourceBuffer, const Buffer &destinationBuffer, vk::DeviceSize size)
            : sourceBuffer(std::move(sourceBuffer)), destinationBuffer(std::move(destinationBuffer)), size(size)
        {
        }
        std::unique_ptr<Buffer> sourceBuffer;
        const Buffer &destinationBuffer;
        vk::DeviceSize size;
    };
    class TransitionImageLayoutCommand
    {
    public:
        TransitionImageLayoutCommand(const vk::raii::Image &image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
        : image(std::move(image)), format(format), oldLayout(oldLayout), newLayout(newLayout)
        {}
        const vk::raii::Image &image;
        vk::Format format;
        vk::ImageLayout oldLayout;
        vk::ImageLayout newLayout;
    };
}