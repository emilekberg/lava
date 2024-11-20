#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <vector>
#include "lava/rendering/screensize.hpp"
#include "lava/rendering/data/texture.hpp"
namespace lava::rendering
{
    class RenderContext
    {
    public:
        RenderContext(const vk::raii::Device &device, const vk::raii::SurfaceKHR &surface, const vk::raii::PhysicalDevice &physicalDevice, const ScreenSize &screenSize);
        ~RenderContext();

        void resize(const ScreenSize &screenSize);

        const vk::raii::SwapchainKHR &getSwapchain() const
        {
            return *_swapchain.get();
        }
        const std::vector<vk::Image> &getImages() const
        {
            return _images;
        }
        const std::vector<vk::raii::ImageView> &getImageViews() const
        {
            return _imageviews;
        }
        vk::Extent2D getExtent() const
        {
            return _extent;
        }
        vk::Format getFormat() const
        {
            return _format;
        }
        const data::Texture& getDepthTexture() const
        {
            return *_depthImage.get();
        }
        const vk::raii::ImageView& getDepthImageView() const
        {
            return _depthImageView;
        }
        vk::Format findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
        vk::Format findDepthFormat() const;
        bool hasStencilComponent(vk::Format format) const;

    private:
        const vk::raii::Device &_device;
        const vk::raii::PhysicalDevice &_physicalDevice;
        const vk::raii::SurfaceKHR &_surface;
        std::unique_ptr<vk::raii::RenderPass> _renderpass;
        std::unique_ptr<vk::raii::SwapchainKHR> _swapchain;
        std::vector<vk::Image> _images;
        std::vector<vk::raii::ImageView> _imageviews;
        std::unique_ptr<data::Texture> _depthImage;
        vk::raii::ImageView _depthImageView;
        vk::Extent2D _extent;
        vk::Format _format;
    };
}