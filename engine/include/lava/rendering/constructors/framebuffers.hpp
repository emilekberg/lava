#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <vector>
namespace lava::rendering::constructors
{
    std::vector<vk::raii::Framebuffer> createFrameBuffers(const vk::raii::Device& device, const vk::raii::RenderPass& renderPass, const vk::Extent2D& extent, const std::vector<vk::raii::ImageView>& imageViews);
}