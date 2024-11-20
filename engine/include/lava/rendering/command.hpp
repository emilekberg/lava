#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "lava/rendering/buffer.hpp"
#include <memory>
namespace lava::rendering
{
    enum CommandType
    {
        NONE,
        COPY_IMAGE_TO_BUFFER,
        COPY_BUFFER,
        TRANSITION,
    };

    struct Command
    {
        CommandType type;
        void* pCommand;
    };
    class CopyImageToBufferCommand
    {
        vk::raii::Buffer& sourceBuffer;
        vk::raii::Image& destinationImage;
        uint32_t width;
        uint32_t height;
    };
    class CopyBufferCommand
    {
    public:
        CopyBufferCommand(std::unique_ptr<Buffer> sourceBuffer, const Buffer& destinationBuffer, vk::DeviceSize size)
            : sourceBuffer(std::move(sourceBuffer)), destinationBuffer(std::move(destinationBuffer)), size(size)
        {}
        std::unique_ptr<Buffer> sourceBuffer;
        const Buffer& destinationBuffer;
        vk::DeviceSize size;
    };
    struct CopyBufferCommand2
    {
        const Buffer& sourceBuffer;
        const Buffer& destinationBuffer;
        vk::DeviceSize size;
    };
}