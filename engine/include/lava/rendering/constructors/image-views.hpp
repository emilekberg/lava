#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <vector>
namespace lava::rendering::constructors
{
    vk::raii::ImageView createImageView(const vk::raii::Device& device, const vk::Image& image, vk::Format format, vk::ImageAspectFlags aspect);
    std::vector<vk::raii::ImageView> createImageViews(const vk::raii::Device& device, const std::vector<vk::Image>& images, vk::Format imageFormat);
}