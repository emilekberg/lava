#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <memory>
namespace lava::rendering::constructors
{
    std::shared_ptr<vk::raii::RenderPass> createRenderPass(const vk::raii::Device& device, const vk::Format& imageFormat);
    vk::raii::RenderPass createRenderPass2(const vk::raii::Device& device, const vk::Format& imageFormat);

}