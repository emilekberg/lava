#include "lava/rendering/constructors/framebuffers.hpp"
namespace lava::rendering::constructors
{
    std::vector<vk::raii::Framebuffer> createFrameBuffers(const vk::raii::Device& device, const vk::raii::RenderPass& renderPass, const vk::Extent2D& extent, const std::vector<vk::raii::ImageView>& imageViews)
    {
        std::vector<vk::raii::Framebuffer> frameBuffers;
        for (auto const &imageview : imageViews)
        {
            std::array<vk::ImageView, 1> attachments;
            attachments[0] = imageview;

            vk::FramebufferCreateInfo framebufferCreateInfo{};
            framebufferCreateInfo.setRenderPass(renderPass);
            framebufferCreateInfo.setAttachmentCount(1);
            framebufferCreateInfo.setPAttachments(attachments.data());
            framebufferCreateInfo.setWidth(extent.width);
            framebufferCreateInfo.setHeight(extent.height);
            framebufferCreateInfo.layers = 1;

            frameBuffers.push_back(vk::raii::Framebuffer(device, framebufferCreateInfo));
        }
        return std::move(frameBuffers);
    }
}