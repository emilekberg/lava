#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <memory>
#include "lava/rendering/render-context.hpp"
namespace lava::rendering
{
    class RenderPass
    {
        public:
            RenderPass(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const RenderContext& renderContext);
            ~RenderPass();

            void recreateFramebuffers(const RenderContext& renderContext);
            void begin(const vk::raii::CommandBuffer& commandBuffer, const RenderContext& renderContext, uint32_t imageIndex);
            void end(const vk::raii::CommandBuffer& commandBuffer);

            const vk::raii::RenderPass& getRenderpass() const
            {
                return *_renderpass.get();
            }
            const std::vector<vk::raii::Framebuffer>& getFramebuffers() const
            {
                return _framebuffers;
            }

        private:
            const vk::raii::Device& _device;
            std::unique_ptr<vk::raii::RenderPass> _renderpass;
            std::vector<vk::raii::Framebuffer> _framebuffers;
    };
}