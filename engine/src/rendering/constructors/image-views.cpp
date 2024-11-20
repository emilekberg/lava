#include "lava/rendering/constructors/image-views.hpp"
namespace lava::rendering::constructors
{
    std::vector<vk::raii::ImageView> createImageViews(const vk::raii::Device& device, const std::vector<vk::Image>& images, vk::Format imageFormat)
    {
        std::vector<vk::raii::ImageView> imageViews;
        for (size_t i = 0; i < images.size(); i++)
        {
            imageViews.push_back(std::move(createImageView(device, images[i], imageFormat, vk::ImageAspectFlagBits::eColor)));
        }
        return imageViews;
    }
    vk::raii::ImageView createImageView(const vk::raii::Device& device, const vk::Image& image, vk::Format format, vk::ImageAspectFlags aspect)
    {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.setImage(image)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(format)
            .subresourceRange
                .setAspectMask(aspect)
                .setBaseMipLevel(0)
                .setLevelCount(1)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

        return std::move(vk::raii::ImageView(device, viewInfo));
    }
}