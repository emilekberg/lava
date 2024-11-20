#include "lava/rendering/renderpass.hpp"
#include "lava/rendering/constructors/framebuffers.hpp"
namespace lava::rendering
{
    RenderPass::RenderPass(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, const RenderContext &renderContext) : _device(device)
    {
        vk::AttachmentDescription colorAttachment{};
        colorAttachment
            .setFormat(renderContext.getFormat())
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)

            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)

            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        vk::AttachmentDescription depthAttachment{};
        depthAttachment
            .setFormat(renderContext.findDepthFormat())
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef
            .setAttachment(0)
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::AttachmentReference depthAttachmentRef{};
        depthAttachmentRef
            .setAttachment(1)
            .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        vk::SubpassDescription subpass{};
        subpass
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setColorAttachmentCount(1)
            .setPColorAttachments(&colorAttachmentRef)
            .setPDepthStencilAttachment(&depthAttachmentRef);

        vk::SubpassDependency dependency{};
        dependency
            .setSrcSubpass(vk::SubpassExternal)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests)
            .setSrcAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

        std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo
            .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
            .setPAttachments(attachments.data())
            .setSubpassCount(1)
            .setPSubpasses(&subpass)
            .setDependencyCount(1)
            .setPDependencies(&dependency);

        _renderpass = std::make_unique<vk::raii::RenderPass>(device, renderPassInfo);
        // _framebuffers = constructors::createFrameBuffers(device, *_renderpass.get(), renderContext.getExtent(), renderContext.getImageViews());
        recreateFramebuffers(renderContext);
    }
    RenderPass::~RenderPass()
    {
    }
    void RenderPass::recreateFramebuffers(const RenderContext &renderContext)
    {
        _framebuffers.clear();
        vk::Extent2D extent = renderContext.getExtent();
        for (auto const &imageview : renderContext.getImageViews())
        {
            std::array<vk::ImageView, 2> attachments =
                {
                    imageview,
                    renderContext.getDepthImageView()};

            vk::FramebufferCreateInfo framebufferCreateInfo{};
            framebufferCreateInfo.setRenderPass(*_renderpass.get());
            framebufferCreateInfo.setAttachmentCount(static_cast<uint32_t>(attachments.size()));
            framebufferCreateInfo.setPAttachments(attachments.data());
            framebufferCreateInfo.setWidth(extent.width);
            framebufferCreateInfo.setHeight(extent.height);
            framebufferCreateInfo.layers = 1;

            _framebuffers.push_back(vk::raii::Framebuffer(_device, framebufferCreateInfo));
        }
    }
    void RenderPass::begin(const vk::raii::CommandBuffer &commandBuffer, const RenderContext &renderContext, uint32_t imageIndex)
    {
        std::array<vk::ClearValue, 2> clearValues{};
        clearValues[0] = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil
            .setDepth(1.0f)
            .setStencil(0);
        vk::RenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo
            .setRenderPass(*_renderpass.get())
            .setFramebuffer(_framebuffers[imageIndex])
            .setRenderArea({0, 0})
            .renderArea.setExtent(renderContext.getExtent());

        renderPassBeginInfo
            .setClearValueCount(static_cast<uint32_t>(clearValues.size()))
            .setPClearValues(clearValues.data());

        commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    }
    void RenderPass::end(const vk::raii::CommandBuffer &commandBuffer)
    {
        commandBuffer.endRenderPass();
    }

}