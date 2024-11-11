#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <vector>
namespace lava::rendering::constructors
{
    std::vector<vk::raii::ImageView> createImageViews(const vk::raii::Device& device, const std::vector<vk::Image>& images, vk::Format imageFormat);
}