#include "lava/app.h"
#include "lava/vulkan/queue-family.cpp"
#include <set>
#include "lava/vulkan/swapchain-support-details.h"
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
        cleanup();
    }

    void App::update()
    {
    }

    void App::render()
    {
    }
    void App::init()
    {
        initWindow();
        initVulkan();
    }

    void App::cleanup()
    {
        _swapchain = nullptr;
        _presentQueue = nullptr;
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
    }

    void App::createDebugMessenger()
    {
        if (!_enableValidationLayers)
            return;
        // vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
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

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        uint32_t glfwExtensionCount = 0;

        auto requiredExtensions = getRequiredExtensions();

        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

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

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        fprintf(stdout, "available extensions:\n");

        for (const auto &extension : extensions)
        {
            fprintf(stdout, "\t%s\n", extension.extensionName);
        }
    }

    void App::createSurface()
    {
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = _window->getWindowHandle();
        createInfo.hinstance = GetModuleHandle(nullptr);
        _surface = std::make_unique<vk::raii::SurfaceKHR>(*_vulkanInstance.get(), createInfo, nullptr);
    }

    void App::pickPhysicalDevice()
    {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        uint32_t deviceCount = 0;

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

        // auto devices = vk::raii::PhysicalDevices(_vulkanInstance.get(), );
        // vkEnumeratePhysicalDevices(_vulkanInstance.get(), &deviceCount, nullptr);
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

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

        createInfo.enabledExtensionCount = 0;

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
        createInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

        vulkan::QueueFamilyIndices indices = vulkan::findQueueFamilies(*_physicalDevice.get(), *_surface.get());
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = vk::True;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        // TODO: fix this

        // createInfo.surface = *_surface.get()

        _swapchain = std::make_unique<vk::raii::SwapchainKHR>(*_device.get(), createInfo, nullptr);
    }

    bool App::checkValidationLayerSupport(const std::vector<const char *> &requiredValidationLayers)
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

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
}
