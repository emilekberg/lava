#include "lava/rendering/render-context.hpp"
#include "lava/rendering/constructors/swapchain.hpp"
#include "lava/rendering/constructors/image-views.hpp"
#include "lava/rendering/constructors/framebuffers.hpp"
#include "lava/rendering/constructors/_renderpass.hpp"
#include <tuple>
namespace lava::rendering
{
    RenderContext::RenderContext(const vk::raii::Device &device, const vk::raii::SurfaceKHR &surface, const vk::raii::PhysicalDevice &physicalDevice, const ScreenSize &screenSize) : _device(device), _physicalDevice(physicalDevice), _surface(surface), _swapchain(nullptr), _depthImageView(VK_NULL_HANDLE)
    {
        std::tie(_swapchain, _format, _extent) = constructors::createSwapChain(device, physicalDevice, surface, screenSize);
        _images = _swapchain->getImages();

        _imageviews = constructors::createImageViews(_device, _images, _format);

        _depthImage = std::make_unique<data::Texture>(device, physicalDevice, _extent.width, _extent.height, findDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment);
        _depthImageView = constructors::createImageView(device, _depthImage->getVkImage(), _depthImage->getFormat(), vk::ImageAspectFlagBits::eDepth);
    }
    RenderContext::~RenderContext()
    {
    }

    void RenderContext::resize(const ScreenSize &screenSize)
    {
        _imageviews.clear();
        _swapchain = nullptr;
        _depthImageView = nullptr;

        std::tie(_swapchain, _format, _extent) = constructors::createSwapChain(_device, _physicalDevice, _surface, screenSize);
        _images = _swapchain->getImages();
        _imageviews = constructors::createImageViews(_device, _images, _format);
        _depthImage = std::make_unique<data::Texture>(_device, _physicalDevice, _extent.width, _extent.height, findDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment);

        _depthImageView = constructors::createImageView(_device, _depthImage->getVkImage(), _depthImage->getFormat(), vk::ImageAspectFlagBits::eDepth);
    }
    vk::Format RenderContext::findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const
    {
        for (vk::Format format : candidates)
        {
            vk::FormatProperties props = _physicalDevice.getFormatProperties(format);
            if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }
    vk::Format RenderContext::findDepthFormat() const
    {
        return findSupportedFormat(
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

    bool RenderContext::hasStencilComponent(vk::Format format) const
    {
        return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
    }

}