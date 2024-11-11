#include "lava/rendering/constructors/swapchain.hpp"
#undef max
namespace lava::rendering::constructors
{
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
    {
        for (const auto &availableFormat : availableFormats)
        {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                return availableFormat;
        }
        return availableFormats[0];
    }
    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes)
    {
        for (const auto &availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox)
                return availablePresentMode;
        }
        return vk::PresentModeKHR::eFifo;
    }
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, const ScreenSize& screenSize)
    {
        constexpr uint32_t max_width{std::numeric_limits<uint32_t>::max()};
        if (capabilities.currentExtent.height != max_width)
        {
            return capabilities.currentExtent;
        }

        // glfwGetFramebufferSize(window.getGLFWwindow(), &width, &height);
        vk::Extent2D actualExtent = {
            static_cast<uint32_t>(screenSize.width),
            static_cast<uint32_t>(screenSize.height)};

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
    std::tuple<std::unique_ptr<vk::raii::SwapchainKHR>, vk::Format, vk::Extent2D> createSwapChain(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR &surface, const ScreenSize& screenSize)
    {
        rendering::SwapChainSupportDetails swapChainSupport = rendering::querySwapChainSupport(physicalDevice, surface);
        vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities, screenSize);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.setSurface(surface);
        createInfo.setMinImageCount(imageCount);
        createInfo.setImageFormat(surfaceFormat.format);
        createInfo.setImageColorSpace(surfaceFormat.colorSpace);
        createInfo.setImageExtent(extent);
        createInfo.setImageArrayLayers(1);
        createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        rendering::QueueFamilyIndices indices = rendering::findQueueFamilies(physicalDevice, surface);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
            createInfo.setQueueFamilyIndexCount(2);
            createInfo.setPQueueFamilyIndices(queueFamilyIndices);
        }
        else
        {
            createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        }

        createInfo.setPreTransform(swapChainSupport.capabilities.currentTransform);
        createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        createInfo.setPresentMode(presentMode);
        createInfo.setClipped(vk::True);

        // use this when recreating the swapchain from the old one.
        // createInfo.setOldSwapchain(VK_NULL_HANDLE);

        auto swapchain = std::make_unique<vk::raii::SwapchainKHR>(device, createInfo, nullptr);
        return std::make_tuple(std::move(swapchain), std::move(surfaceFormat.format), extent);
    }
    
    std::tuple<vk::raii::SwapchainKHR, vk::Format, vk::Extent2D> createSwapChain2(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR &surface, const ScreenSize& screenSize)
    {
        rendering::SwapChainSupportDetails swapChainSupport = rendering::querySwapChainSupport(physicalDevice, surface);
        vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities, screenSize);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.setSurface(surface);
        createInfo.setMinImageCount(imageCount);
        createInfo.setImageFormat(surfaceFormat.format);
        createInfo.setImageColorSpace(surfaceFormat.colorSpace);
        createInfo.setImageExtent(extent);
        createInfo.setImageArrayLayers(1);
        createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        rendering::QueueFamilyIndices indices = rendering::findQueueFamilies(physicalDevice, surface);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
            createInfo.setQueueFamilyIndexCount(2);
            createInfo.setPQueueFamilyIndices(queueFamilyIndices);
        }
        else
        {
            createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        }

        createInfo.setPreTransform(swapChainSupport.capabilities.currentTransform);
        createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        createInfo.setPresentMode(presentMode);
        createInfo.setClipped(vk::True);

        // use this when recreating the swapchain from the old one.
        // createInfo.setOldSwapchain(VK_NULL_HANDLE);

        auto swapchain = vk::raii::SwapchainKHR(device, createInfo, nullptr);
        return std::make_tuple(std::move(swapchain), std::move(surfaceFormat.format), extent);
    }

    std::tuple<std::unique_ptr<vk::raii::SwapchainKHR>, vk::Extent2D> recreateSwapChain(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR &surface, const ScreenSize& screenSize, const vk::raii::SwapchainKHR& oldSwapChain)
    {
        rendering::SwapChainSupportDetails swapChainSupport = rendering::querySwapChainSupport(physicalDevice, surface);
        vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities, screenSize);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.setSurface(surface);
        createInfo.setImageExtent(extent);

        // use this when recreating the swapchain from the old one.
        createInfo.setOldSwapchain(oldSwapChain);

        auto swapchain = std::make_unique<vk::raii::SwapchainKHR>(device, createInfo, nullptr);
        return std::make_tuple(std::move(swapchain), extent);
    }
}