#include "lava/app.h"
#include "lava/vulkan/queue-family.cpp"
#include <set>
#include "lava/vulkan/swapchain-support-details.h"
#include "lava/resourceloader.h"
#undef max
namespace lava
{
    App::App()
    {
        _validationLayers = {
            "VK_LAYER_KHRONOS_validation"};
        _deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        _vertices = {
            // top triangle
            {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

            // bottom triangle
            
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},


            };
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

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *_pipeline.get());

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

        commandBuffer.draw(static_cast<uint32_t>(_vertices.size()), 1, 0, 0);
        //commandBuffer.draw(3, 1, 0, 0);

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
        _pipeline = nullptr;
        _pipelineLayout = nullptr;
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
        createInstance();
        createDebugMessenger();
        createSurface();
        pickPhysicalDevice();

        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFrameBuffers();
        createCommandPool();
        createVertexBuffers();
        createCommandBuffer();
        createSyncObjects();
    }

    void App::createDebugMessenger()
    {
        if (!_enableValidationLayers)
            return;
        vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
        createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional

        _debugMessenger = std::make_unique<vk::raii::DebugUtilsMessengerEXT>(*_vulkanInstance, createInfo, nullptr);
        if (_debugMessenger == nullptr)
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    void App::createInstance()
    {
        if (_enableValidationLayers && !checkValidationLayerSupport(_validationLayers))
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        vk::raii::Context context;
        uint32_t apiVersion = context.enumerateInstanceVersion();

        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        vk::InstanceCreateInfo createInfo{};
        createInfo.pApplicationInfo = &appInfo;
        uint32_t glfwExtensionCount = 0;

        auto requiredExtensions = getRequiredExtensions();

        createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        if (_enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
            createInfo.ppEnabledLayerNames = _validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        _vulkanInstance = std::make_unique<vk::raii::Instance>(context, createInfo);
        if (_vulkanInstance == nullptr)
        {
            throw std::runtime_error("failed to create vulkan instance");
        }
        std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();
        fprintf(stdout, "available extensions:\n");

        for (const auto &extension : extensions)
        {
            fprintf(stdout, "\t%s\n", extension.extensionName);
        }
    }

    void App::createSurface()
    {
        vk::Win32SurfaceCreateInfoKHR createInfo{};
        createInfo.hwnd = _window->getWindowHandle();
        createInfo.hinstance = GetModuleHandle(nullptr);
        _surface = std::make_unique<vk::raii::SurfaceKHR>(*_vulkanInstance.get(), createInfo);
    }

    void App::pickPhysicalDevice()
    {
        auto devices = vk::raii::PhysicalDevices(*_vulkanInstance.get());
        if (devices.size() == 0)
        {
            throw std::runtime_error("failed to find GPUs with vulkan support");
        }

        // find suitable device
        std::optional<vk::raii::PhysicalDevice> selectedPhysicalDevice;
        for (const auto &device : devices)
        {
            if (isDeviceSuitable(device))
            {
                selectedPhysicalDevice = std::move(device);
                break;
            }
        }
        if (!selectedPhysicalDevice.has_value())
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        _physicalDevice = std::make_unique<vk::raii::PhysicalDevice>(std::move(selectedPhysicalDevice.value()));
        fprintf(stdout, "Selected device: %s\n", _physicalDevice->getProperties().deviceName);
    }

    bool App::isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice)
    {
        auto queueFamilyIndices = vulkan::findQueueFamilies(physicalDevice, *_surface.get());
        if (!queueFamilyIndices.isComplete())
            return false;
        if (physicalDevice.getProperties().deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
            return false;
        if (!physicalDevice.getFeatures().geometryShader)
            return false;
        if (!checkDeviceExtensionsSupport(physicalDevice))
            return false;
        lava::vulkan::SwapChainSupportDetails swapChainDetails = lava::vulkan::querySwapChainSupport(physicalDevice, *_surface.get());
        if (!swapChainDetails.isAdequate())
            return false;
        return true;
    }

    bool App::checkDeviceExtensionsSupport(const vk::raii::PhysicalDevice &physicalDevice)
    {
        auto availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
        std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

        for (const auto &extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }
        if (!requiredExtensions.empty())
        {
            fprintf(stderr, "missing required extensions!\n");
            // TODO: loop and log the required extensions.
            return false;
        }
        return true;
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
        vulkan::QueueFamilyIndices indices = vulkan::findQueueFamilies(*_physicalDevice.get(), *_surface.get());

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
        vulkan::SwapChainSupportDetails swapChainSupport = vulkan::querySwapChainSupport(*_physicalDevice.get(), *_surface.get());
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

        vulkan::QueueFamilyIndices indices = vulkan::findQueueFamilies(*_physicalDevice.get(), *_surface.get());
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

        _renderpass = std::make_unique<vk::raii::RenderPass>(*_device.get(), renderPassInfo);
    }

    void App::createGraphicsPipeline()
    {
        auto vertShaderCode = lava::resourceloader::readfile("./build/shaders/shader_vert.spv");
        auto fragShaderCode = lava::resourceloader::readfile("./build/shaders/shader_frag.spv");

        vk::raii::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        vk::raii::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);
        vertShaderStageInfo.setModule(vertShaderModule);
        vertShaderStageInfo.setPName("main");

        vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);
        fragShaderStageInfo.setModule(fragShaderModule);
        fragShaderStageInfo.setPName("main");

        vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eViewport, vk::DynamicState::eScissor};

        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));
        dynamicState.setPDynamicStates(dynamicStates.data());

        auto bindingDescription = data::Vertex::getBindingDescription();
        auto attributeDescriptions = data::Vertex::getAttributeDescriptions();

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.setVertexBindingDescriptionCount(1);
        vertexInputInfo.setPVertexBindingDescriptions(&bindingDescription);
        vertexInputInfo.setVertexAttributeDescriptionCount(attributeDescriptions.size());
        vertexInputInfo.setVertexAttributeDescriptions(attributeDescriptions);

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
        inputAssembly.setPrimitiveRestartEnable(vk::False);

        vk::Viewport viewport{};
        viewport.setX(0.0f);
        viewport.setY(0.0f);
        viewport.setWidth((float)_swapchainExtent.width);
        viewport.setHeight((float)_swapchainExtent.height);
        viewport.setMinDepth(0.0f);
        viewport.setMaxDepth(1.0f);

        vk::Rect2D scissor{};
        scissor.setOffset({0, 0});
        scissor.setExtent(_swapchainExtent);

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.setViewportCount(1);
        viewportState.setPViewports(&viewport);
        viewportState.setScissorCount(1);
        viewportState.setPScissors(&scissor);

        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.setDepthClampEnable(vk::False);
        rasterizer.setRasterizerDiscardEnable(vk::False);
        rasterizer.setPolygonMode(vk::PolygonMode::eFill);
        rasterizer.setLineWidth(1.0f);

        rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
        rasterizer.setFrontFace(vk::FrontFace::eClockwise);

        rasterizer.setDepthBiasEnable(vk::False);
        rasterizer.setDepthBiasConstantFactor(0.0f);
        rasterizer.setDepthBiasClamp(0.0f);
        rasterizer.setDepthBiasSlopeFactor(0.0f);

        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.setSampleShadingEnable(vk::False);
        multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
        multisampling.setMinSampleShading(1.0f);           // Optional
        multisampling.setPSampleMask(nullptr);             // Optional
        multisampling.setAlphaToCoverageEnable(vk::False); // Optional
        multisampling.setAlphaToOneEnable(vk::False);      // Optional

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
        colorBlendAttachment.setBlendEnable(vk::False);
        colorBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eOne);  // Optional
        colorBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eZero); // Optional
        colorBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);             // Optional
        colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eOne);  // Optional
        colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero); // Optional
        colorBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);             // Optional

        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.setLogicOpEnable(vk::False);
        colorBlending.setAttachmentCount(1);
        colorBlending.setPAttachments(&colorBlendAttachment);

        vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.setSetLayoutCount(0);
        pipelineLayoutCreateInfo.setPSetLayouts(nullptr);

        _pipelineLayout = std::make_unique<vk::raii::PipelineLayout>(*_device.get(), pipelineLayoutCreateInfo);

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.setStageCount(2);
        pipelineInfo.setPStages(shaderStages);
        pipelineInfo.setPVertexInputState(&vertexInputInfo);
        pipelineInfo.setPInputAssemblyState(&inputAssembly);
        pipelineInfo.setPViewportState(&viewportState);
        pipelineInfo.setPRasterizationState(&rasterizer);
        pipelineInfo.setPMultisampleState(&multisampling);
        pipelineInfo.setPDepthStencilState(nullptr);
        pipelineInfo.setPColorBlendState(&colorBlending);
        pipelineInfo.setPDynamicState(&dynamicState);

        pipelineInfo.setLayout(*_pipelineLayout.get());
        pipelineInfo.setRenderPass(*_renderpass.get());

        pipelineInfo.setSubpass(0);

        _pipeline = std::make_unique<vk::raii::Pipeline>(*_device.get(), nullptr, pipelineInfo);
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
        vulkan::QueueFamilyIndices queueFamilyIndices = vulkan::findQueueFamilies(*_physicalDevice.get(), *_surface.get());

        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        poolInfo.setQueueFamilyIndex(queueFamilyIndices.graphicsFamily.value());

        _commandPool = std::make_unique<vk::raii::CommandPool>(*_device.get(), poolInfo);
    }

    void App::createVertexBuffers()
    {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(sizeof(_vertices[0]) * _vertices.size());
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

        void* data = _vertexBufferMemory->mapMemory(0, bufferInfo.size);
        memcpy(data, _vertices.data(), (size_t)bufferInfo.size);
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

    bool App::checkValidationLayerSupport(const std::vector<const char *> &requiredValidationLayers)
    {
        std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

        fprintf(stdout, "using validation layers:\n");
        for (const char *layerName : requiredValidationLayers)
        {
            bool layerFound = false;
            fprintf(stdout, "\t%s:\n", layerName);
            for (const auto &layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                fprintf(stdout, "\t\tnot found, validation not supported:\n");
                return false;
            }
        }

        return true;
    }
    std::vector<const char *> App::getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (_enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        return extensions;
    }

    void App::setFrameBufferResized()
    {
        _framebufferResized = true;
    }
    
    uint32_t App::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memoryProperties = _physicalDevice->getMemoryProperties();
        for(size_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if(typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        throw std::runtime_error("failed to find suitable memory type!");
    }


    VKAPI_ATTR VkBool32 VKAPI_CALL App::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
    {
        if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            return VK_FALSE;
        }
        fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);

        return VK_FALSE;
    }

    VKAPI_ATTR vk::Bool32 VKAPI_CALL App::vkdebugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
    {
        if (messageSeverity < vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        {
            return vk::False;
        }
        fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);

        return vk::False;
    }

    void App::handleWindowResize(GLFWwindow *window, int width, int height)
    {
        auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
        app->setFrameBufferResized();
    }

}
