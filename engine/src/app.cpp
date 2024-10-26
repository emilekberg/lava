#include "lava/app.h"

namespace lava
{
    App::App()
    {
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
        _window = nullptr;
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
    }

    void App::createDebugMessenger()
    {
        if (!_enableValidationLayers)
            return;
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
        const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"};

        if (_enableValidationLayers && !checkValidationLayerSupport(validationLayers))
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
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
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
