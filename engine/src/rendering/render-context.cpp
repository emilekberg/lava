#include "lava/rendering/render-context.hpp"
#include "lava/rendering/constructors/swapchain.hpp"
#include "lava/rendering/constructors/image-views.hpp"
#include "lava/rendering/constructors/framebuffers.hpp"
#include "lava/rendering/constructors/_renderpass.hpp"
#include <tuple>
namespace lava::rendering
{
    RenderContext::RenderContext(const vk::raii::Device& device, const vk::raii::SurfaceKHR& surface, const vk::raii::PhysicalDevice& physicalDevice, const ScreenSize& screenSize) :
        _device(device), _physicalDevice(physicalDevice), _surface(surface), _swapchain(nullptr)
    {
        std::tie(_swapchain, _format, _extent) = constructors::createSwapChain(device, physicalDevice, surface, screenSize);       
        _images = _swapchain->getImages();

        _imageviews = constructors::createImageViews(_device, _images, _format);
        // _renderpass = std::make_unique<vk::raii::RenderPass>(constructors::createRenderPass2(_device, _format));
        // _framebuffers = constructors::createFrameBuffers(_device, *_renderpass.get(), _extent, _imageviews);
    }
    RenderContext::~RenderContext()
    {

    }

    void RenderContext::resize(const ScreenSize& screenSize)
    {
        _framebuffers.clear();
        _imageviews.clear();
        _swapchain = nullptr;
        
        std::tie(_swapchain, _format, _extent) = constructors::createSwapChain(_device, _physicalDevice, _surface, screenSize);       
        _images = _swapchain->getImages();
        _imageviews = constructors::createImageViews(_device, _images, _format);
        // _framebuffers = constructors::createFrameBuffers(_device, *_renderpass.get(), _extent, _imageviews);
    }
}