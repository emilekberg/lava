#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <memory>
namespace lava::rendering::constructors
{
    std::shared_ptr<vk::raii::RenderPass> createRenderPass(const vk::raii::Device& device, const vk::Format& imageFormat)
    {
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.setFormat(imageFormat);
        colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
        colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
        colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);

        colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);

        colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
        colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.setAttachment(0);
        colorAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::SubpassDescription subpass{};
        subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
        subpass.colorAttachmentCount = 1;
        subpass.setPColorAttachments(&colorAttachmentRef);

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.setAttachmentCount(1);
        renderPassInfo.setPAttachments(&colorAttachment);
        renderPassInfo.setSubpassCount(1);
        renderPassInfo.setPSubpasses(&subpass);

        vk::SubpassDependency dependency{};
        dependency.setSrcSubpass(vk::SubpassExternal);
        dependency.setDstSubpass(0);
        dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        // dependency.setSrcAccessMask();
        dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
        renderPassInfo.setDependencyCount(1);
        renderPassInfo.setPDependencies(&dependency);

        return std::make_shared<vk::raii::RenderPass>(device, renderPassInfo);
    }
}