#include "lava/app.hpp"
#include <set>
#include "lava/rendering/queue-family.hpp"
#include "lava/rendering/swapchain-support-details.hpp"
#include "lava/resourceloader.hpp"
#include "lava/rendering/graphics-pipeline-builder.hpp"
#include "lava/rendering/shader.hpp"
#include "lava/rendering/attribute-description-builder.hpp"
#include "lava/rendering/constructors/instance.hpp"
#include "lava/rendering/constructors/debug-messenger.hpp"
#include "lava/rendering/constructors/surface.hpp"
#include "lava/rendering/constructors/physical-device.hpp"
#include "lava/rendering/constructors/device.hpp"

#define UNHANDLED_PARAMETER(param) param;
#undef max
namespace lava
{
    App::App()
    {
        _validationLayers = {
            "VK_LAYER_KHRONOS_validation"};
        _deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    }
    App::~App()
    {
        cleanup();
    }
    void App::run()
    {
        init();

        while (!_window->shouldClose())
        {
            _window->pollEvents();
            update();
            render();
        }
        _device->waitIdle();
        cleanup();
    }

    void App::update()
    {
    }

    void App::render()
    {
        vk::Result result = _device->waitForFences({*_inFlightFence[_currentFrame]}, vk::True, UINT64_MAX);

        uint32_t imageIndex;

#ifndef VULKAN_HPP_NO_EXCEPTIONS
        try
#endif
        {
            std::tie(result, imageIndex) = _swapchain->acquireNextImage(UINT64_MAX, *_imageAvailableSemaphore[_currentFrame]);
            if (result == vk::Result::eErrorOutOfDateKHR)
            {
                _framebufferResized = true;
            }
            else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
            {
                throw std::runtime_error("failed to acquire swap chain image!");
            }
        }
#ifndef VULKAN_HPP_NO_EXCEPTIONS
        catch (vk::OutOfDateKHRError &e)
        {
            UNHANDLED_PARAMETER(e)
            _framebufferResized = true;
        }
#endif
        if (_framebufferResized)
        {
            _framebufferResized = false;
            recreateSwapChain();
            return;
        }
        _device->resetFences({*_inFlightFence[_currentFrame]});

        _commandBuffers->at(_currentFrame).reset();

        recordCommandBuffer(_commandBuffers->at(_currentFrame), imageIndex);

        vk::SubmitInfo submitInfo = {};

        vk::Semaphore waitSemaphore[] = {*_imageAvailableSemaphore[_currentFrame]};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.setWaitSemaphoreCount(1);
        submitInfo.setPWaitSemaphores(waitSemaphore);
        submitInfo.setPWaitDstStageMask(waitStages);

        vk::CommandBuffer commandBuffers[] = {_commandBuffers->at(_currentFrame)};
        submitInfo.setCommandBufferCount(1);
        submitInfo.setCommandBuffers(commandBuffers);

        vk::Semaphore signalSemaphores[] = {*_renderFinishedSemaphore[_currentFrame]};
        submitInfo.setSignalSemaphoreCount(1);
        submitInfo.setPSignalSemaphores(signalSemaphores);

        _graphicsQueue->submit({submitInfo}, *_inFlightFence[_currentFrame]);

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphoreCount(1);
        presentInfo.setPWaitSemaphores(signalSemaphores);

        vk::SwapchainKHR swapChains[] = {*_swapchain.get()};
        presentInfo.setSwapchainCount(1);
        presentInfo.setPSwapchains(swapChains);
        presentInfo.setPImageIndices(&imageIndex);

#ifndef VULKAN_HPP_NO_EXCEPTIONS
        try
#endif
        {

            result = _presentQueue->presentKHR(presentInfo);
            if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
            {
                _framebufferResized = true;
            }
            else if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error("failed to present swap chain image!");
            }
        }
#ifndef VULKAN_HPP_NO_EXCEPTIONS
        catch (const vk::OutOfDateKHRError &e)
        {
            UNHANDLED_PARAMETER(e)
            _framebufferResized = true;
        }
#endif
        _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        if (_framebufferResized)
        {
            _framebufferResized = false;
            recreateSwapChain();
        }
    }
    void App::recordCommandBuffer(const vk::raii::CommandBuffer &commandBuffer, uint32_t imageIndex)
    {
        vk::CommandBufferBeginInfo beginInfo{};
        // beginInfo.setFlags();
        // beginInfo.setPInheritanceInfo(nullptr);

        commandBuffer.begin(beginInfo);

        vk::RenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.setRenderPass(*_renderpass.get());
        renderPassBeginInfo.setFramebuffer(_swapchainFrameBuffers[imageIndex]);
        renderPassBeginInfo.setRenderArea({0, 0});
        renderPassBeginInfo.renderArea.setExtent(_swapchainExtent);

        vk::ClearValue clearColor = {{0.0f, 0.0f, 0.0f, 1.0f}};
        renderPassBeginInfo.setClearValueCount(1);
        renderPassBeginInfo.setPClearValues(&clearColor);

        commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _graphicsPipeline->getVkPipeline());

        vk::Viewport viewport{};
        viewport.setX(0.0f);
        viewport.setY(0.0f);
        viewport.setWidth(static_cast<float>(_swapchainExtent.width));
        viewport.setHeight(static_cast<float>(_swapchainExtent.height));
        viewport.setMinDepth(0.0f);
        viewport.setMaxDepth(0.0f);
        commandBuffer.setViewport(0, viewport);

        vk::Rect2D scissor{};
        scissor.setOffset({0, 0});
        scissor.setExtent(_swapchainExtent);
        commandBuffer.setScissor(0, scissor);

        vk::Buffer vertexBuffers[] = {*_vertexBuffer.get()};
        vk::DeviceSize offsets[] = {0};
        commandBuffer.bindVertexBuffers(0, vertexBuffers, offsets);

        commandBuffer.draw(static_cast<uint32_t>(_mesh.vertices.size()), 1, 0, 0);
        // commandBuffer.draw(3, 1, 0, 0);

        commandBuffer.endRenderPass();
        commandBuffer.end();
    }
    void App::init()
    {
        initWindow();
        initVulkan();
    }

    void App::cleanupSwapChain()
    {
        _swapchainFrameBuffers.clear();
        _swapchainImageViews.clear();
        _swapchain = nullptr;
    }

    void App::cleanup()
    {
        cleanupSwapChain();
        _vertexBuffer = nullptr;
        _vertexBufferMemory = nullptr;
        _inFlightFence.clear();
        _commandBuffers->clear();
        _commandPool = nullptr;
        _graphicsPipeline = nullptr;
        // _pipeline = nullptr;
        // _pipelineLayout = nullptr;
        _renderpass = nullptr;

        _renderFinishedSemaphore.clear();
        _imageAvailableSemaphore.clear();

        _swapchainImages.clear();
        _swapchain = nullptr;
        _presentQueue = nullptr;
        _graphicsQueue = nullptr;
        _device = nullptr;
        _physicalDevice = nullptr;
        _window = nullptr;
        _debugMessenger = nullptr;
        _surface = nullptr;
        _vulkanInstance = nullptr;
    }

    void App::initWindow()
    {
        _window = std::make_unique<core::Window>();
        glfwSetWindowUserPointer(_window->getGLFWwindow(), this);
        _window->setResizeHandler(handleWindowResize);
        _window->activate();
    }

    void App::initVulkan()
    {
        _vulkanInstance = rendering::constructors::createInstance(_enableValidationLayers, _validationLayers);
        _debugMessenger = rendering::constructors::createDebugMessenger(*_vulkanInstance.get());
        _surface = rendering::constructors::createSurface(*_vulkanInstance.get(), _window->getWindowHandle());
        _physicalDevice = rendering::constructors::pickPhysicalDevice(*_vulkanInstance.get(), *_surface.get(), _deviceExtensions);
        
        std::tie(_device, _presentQueue, _graphicsQueue) = rendering::constructors::createDevice(*_vulkanInstance.get(), *_physicalDevice.get(), *_surface.get(), _deviceExtensions, _validationLayers);
        
        createSwapChain();
        createImageViews();
        createRenderPass();
        _graphicsPipeline = rendering::GraphicsPipelineBuilder(*_device.get())
            .withFragmentShader()
            .withVertexShader()
            .withVertexInputInfo()
            .withExtent(_swapchainExtent)
            .withRenderPass(_renderpass)
            .build();
        createFrameBuffers();
        createCommandPool();
        createVertexBuffers();
        createCommandBuffer();
        createSyncObjects();
    }

    vk::SurfaceFormatKHR App::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
    {
        for (const auto &availableFormat : availableFormats)
        {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                return availableFormat;
        }
        return availableFormats[0];
    }
    vk::PresentModeKHR App::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes)
    {
        for (const auto &availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox)
                return availablePresentMode;
        }
        return vk::PresentModeKHR::eFifo;
    }
    vk::Extent2D App::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
    {
        constexpr uint32_t max_width{std::numeric_limits<uint32_t>::max()};
        if (capabilities.currentExtent.height != max_width)
        {
            return capabilities.currentExtent;
        }
        int width, height;
        glfwGetFramebufferSize(_window->getGLFWwindow(), &width, &height);
        vk::Extent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)};

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    void App::createLogicalDevice()
    {
        rendering::QueueFamilyIndices indices = rendering::findQueueFamilies(*_physicalDevice.get(), *_surface.get());

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            vk::DeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::PhysicalDeviceFeatures deviceFeatures{};

        vk::DeviceCreateInfo createInfo{};
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

        if (_enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
            createInfo.ppEnabledLayerNames = _validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        _device = std::make_unique<vk::raii::Device>(std::move(_physicalDevice->createDevice(createInfo)));
        _presentQueue = std::make_unique<vk::raii::Queue>(std::move(_device->getQueue(indices.presentFamily.value(), 0)));
        _graphicsQueue = std::make_unique<vk::raii::Queue>(std::move(_device->getQueue(indices.graphicsFamily.value(), 0)));
    }

    void App::createSwapChain()
    {
        rendering::SwapChainSupportDetails swapChainSupport = rendering::querySwapChainSupport(*_physicalDevice.get(), *_surface.get());
        vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.setSurface(*_surface.get());
        createInfo.setMinImageCount(imageCount);
        createInfo.setImageFormat(surfaceFormat.format);
        createInfo.setImageColorSpace(surfaceFormat.colorSpace);
        createInfo.setImageExtent(extent);
        createInfo.setImageArrayLayers(1);
        createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        rendering::QueueFamilyIndices indices = rendering::findQueueFamilies(*_physicalDevice.get(), *_surface.get());
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

        _swapchain = std::make_unique<vk::raii::SwapchainKHR>(*_device.get(), createInfo, nullptr);
        _swapchainImages = _swapchain->getImages();
        _swapchainImageFormat = surfaceFormat.format;
        _swapchainExtent = extent;
    }

    void App::recreateSwapChain()
    {
        // handle case where the window is minimized. the width and height is 0 in this case.
        int width = 0, height = 0;
        glfwGetFramebufferSize(_window->getGLFWwindow(), &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(_window->getGLFWwindow(), &width, &height);
            _window->pollEvents();
        }

        _device->waitIdle();
        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createFrameBuffers();
    }

    void App::createImageViews()
    {
        for (size_t i = 0; i < _swapchainImages.size(); i++)
        {
            vk::ImageViewCreateInfo createInfo{};
            createInfo.setImage(_swapchainImages[i]);
            createInfo.setViewType(vk::ImageViewType::e2D);
            createInfo.setFormat(_swapchainImageFormat);
            createInfo.components.setR(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setG(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setB(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setA(vk::ComponentSwizzle::eIdentity);

            createInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
            createInfo.subresourceRange.setBaseMipLevel(0);
            createInfo.subresourceRange.setLevelCount(1);
            createInfo.subresourceRange.setBaseArrayLayer(0);
            createInfo.subresourceRange.setLayerCount(1);

            vk::raii::ImageView imageView(*_device.get(), createInfo);

            _swapchainImageViews.push_back(std::move(imageView));
        }
    }

    vk::raii::ShaderModule App::createShaderModule(const std::vector<char> &code)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.setCodeSize(code.size());
        createInfo.setPCode(reinterpret_cast<const uint32_t *>(code.data()));
        return vk::raii::ShaderModule(*_device.get(), createInfo);
    }

    void App::createRenderPass()
    {
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.setFormat(_swapchainImageFormat);
        colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
        colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
        colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);

        colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);

        colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
        colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.setAttachment(0);
        colorAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::SubpassDescription subpass{};
        subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
        subpass.colorAttachmentCount = 1;
        subpass.setPColorAttachments(&colorAttachmentRef);

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.setAttachmentCount(1);
        renderPassInfo.setPAttachments(&colorAttachment);
        renderPassInfo.setSubpassCount(1);
        renderPassInfo.setPSubpasses(&subpass);

        vk::SubpassDependency dependency{};
        dependency.setSrcSubpass(vk::SubpassExternal);
        dependency.setDstSubpass(0);
        dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        // dependency.setSrcAccessMask();
        dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
        renderPassInfo.setDependencyCount(1);
        renderPassInfo.setPDependencies(&dependency);

        _renderpass = std::make_shared<vk::raii::RenderPass>(*_device.get(), renderPassInfo);
    }

    void App::createGraphicsPipeline()
    {
        _graphicsPipeline = rendering::GraphicsPipelineBuilder(*_device.get())
            .withFragmentShader()
            .withVertexShader()
            .withVertexInputInfo()
            .withExtent(_swapchainExtent)
            .withRenderPass(_renderpass)
            .build();
    }

    void App::createFrameBuffers()
    {
        // _swapchainFrameBuffers.resize(_swapchainImageViews.size());
        for (auto const &imageview : _swapchainImageViews)
        {
            std::array<vk::ImageView, 1> attachments;
            attachments[0] = imageview;

            vk::FramebufferCreateInfo framebufferCreateInfo{};
            framebufferCreateInfo.setRenderPass(*_renderpass.get());
            framebufferCreateInfo.setAttachmentCount(1);
            framebufferCreateInfo.setPAttachments(attachments.data());
            framebufferCreateInfo.setWidth(_swapchainExtent.width);
            framebufferCreateInfo.setHeight(_swapchainExtent.height);
            framebufferCreateInfo.layers = 1;

            auto fb = vk::raii::Framebuffer(*_device.get(), framebufferCreateInfo);
            _swapchainFrameBuffers.push_back(std::move(fb));
        }
    }

    void App::createCommandPool()
    {
        rendering::QueueFamilyIndices queueFamilyIndices = rendering::findQueueFamilies(*_physicalDevice.get(), *_surface.get());

        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        poolInfo.setQueueFamilyIndex(queueFamilyIndices.graphicsFamily.value());

        _commandPool = std::make_unique<vk::raii::CommandPool>(*_device.get(), poolInfo);
    }

    void App::createVertexBuffers()
    {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(sizeof(_mesh.vertices[0]) * _mesh.vertices.size());
        bufferInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
        bufferInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
        bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

        _vertexBuffer = std::make_unique<vk::raii::Buffer>(*_device.get(), bufferInfo);

        auto memoryRequirements = _vertexBuffer->getMemoryRequirements();

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.setAllocationSize(memoryRequirements.size);
        allocInfo.setMemoryTypeIndex(findMemoryType(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

        _vertexBufferMemory = std::make_unique<vk::raii::DeviceMemory>(*_device.get(), allocInfo);
        _vertexBuffer->bindMemory(*_vertexBufferMemory.get(), 0);

        void *data = _vertexBufferMemory->mapMemory(0, bufferInfo.size);
        memcpy(data, _mesh.vertices.data(), (size_t)bufferInfo.size);
        _vertexBufferMemory->unmapMemory();
    }

    void App::createCommandBuffer()
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setCommandPool(*_commandPool.get());
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        allocInfo.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);

        _commandBuffers = std::make_unique<vk::raii::CommandBuffers>(*_device.get(), allocInfo);
    }

    void App::createSyncObjects()
    {
        vk::SemaphoreCreateInfo semaphoreInfo{};
        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            _imageAvailableSemaphore.push_back(vk::raii::Semaphore(*_device.get(), semaphoreInfo));
            _renderFinishedSemaphore.push_back(vk::raii::Semaphore(*_device.get(), semaphoreInfo));
            _inFlightFence.push_back(vk::raii::Fence(*_device.get(), fenceInfo));
        }
    }

    void App::setFrameBufferResized()
    {
        _framebufferResized = true;
    }

    uint32_t App::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memoryProperties = _physicalDevice->getMemoryProperties();
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        throw std::runtime_error("failed to find suitable memory type!");
    }

    void App::handleWindowResize(GLFWwindow *window, int width, int height)
    {
        auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
        app->setFrameBufferResized();
    }

}
