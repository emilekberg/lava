#include "lava/rendering/constructors/image-views.hpp"
namespace lava::rendering::constructors
{
    std::vector<vk::raii::ImageView> createImageViews(const vk::raii::Device& device, const std::vector<vk::Image>& images, vk::Format imageFormat)
    {
        std::vector<vk::raii::ImageView> imageViews;
        for (size_t i = 0; i < images.size(); i++)
        {
            vk::ImageViewCreateInfo createInfo{};
            createInfo.setImage(images[i]);
            createInfo.setViewType(vk::ImageViewType::e2D);
            createInfo.setFormat(imageFormat);
            createInfo.components.setR(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setG(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setB(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setA(vk::ComponentSwizzle::eIdentity);

            createInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
            createInfo.subresourceRange.setBaseMipLevel(0);
            createInfo.subresourceRange.setLevelCount(1);
            createInfo.subresourceRange.setBaseArrayLayer(0);
            createInfo.subresourceRange.setLayerCount(1);

            vk::raii::ImageView imageView(device, createInfo);

            imageViews.push_back(std::move(imageView));
        }
        return imageViews;
    }
}