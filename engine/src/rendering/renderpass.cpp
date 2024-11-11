#include "lava/rendering/renderpass.hpp"
#include "lava/rendering/constructors/framebuffers.hpp"
namespace lava::rendering
{
    RenderPass::RenderPass(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, const RenderContext &renderContext) : _device(device)
    {
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.setFormat(renderContext.getFormat())
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)

            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)

            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.setAttachment(0)
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::SubpassDescription subpass{};
        subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setPColorAttachments(&colorAttachmentRef)
            .setColorAttachmentCount(1);

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.setAttachmentCount(1)
            .setPAttachments(&colorAttachment)
            .setSubpassCount(1)
            .setPSubpasses(&subpass);

        vk::SubpassDependency dependency{};
        dependency.setSrcSubpass(vk::SubpassExternal)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            // dependency.setSrcAccessMask();
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

        renderPassInfo.setDependencyCount(1);
        renderPassInfo.setPDependencies(&dependency);

        _renderpass = std::make_unique<vk::raii::RenderPass>(device, renderPassInfo);
        // _framebuffers = constructors::createFrameBuffers(device, *_renderpass.get(), renderContext.getExtent(), renderContext.getImageViews());
        recreateFramebuffers(renderContext);
    }
    RenderPass::~RenderPass()
    {
    }
    void RenderPass::recreateFramebuffers(const RenderContext &renderContext)
    {
        vk::Extent2D extent = renderContext.getExtent();
        for (auto const &imageview : renderContext.getImageViews())
        {
            std::array<vk::ImageView, 1> attachments;
            attachments[0] = imageview;

            vk::FramebufferCreateInfo framebufferCreateInfo{};
            framebufferCreateInfo.setRenderPass(*_renderpass.get());
            framebufferCreateInfo.setAttachmentCount(1);
            framebufferCreateInfo.setPAttachments(attachments.data());
            framebufferCreateInfo.setWidth(extent.width);
            framebufferCreateInfo.setHeight(extent.height);
            framebufferCreateInfo.layers = 1;

            _framebuffers.push_back(vk::raii::Framebuffer(_device, framebufferCreateInfo));
        }
    }
    void RenderPass::begin(const vk::raii::CommandBuffer &commandBuffer, const RenderContext &renderContext, uint32_t imageIndex)
    {
        vk::ClearValue clearColor = {{0.0f, 0.0f, 0.0f, 1.0f}};
        vk::RenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo
            .setRenderPass(*_renderpass.get())
            .setFramebuffer(_framebuffers[imageIndex])
            .setRenderArea({0, 0})
            .renderArea.setExtent(renderContext.getExtent());

        renderPassBeginInfo
            .setClearValueCount(1)
            .setPClearValues(&clearColor);

        commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    }
    void RenderPass::end(const vk::raii::CommandBuffer &commandBuffer)
    {
        commandBuffer.endRenderPass();
    }

}