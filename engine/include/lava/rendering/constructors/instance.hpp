#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <memory>
#include <vector>
namespace lava::rendering::constructors
{
    std::vector<const char *> getRequiredExtensions(bool enableValidationLayers)
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        return extensions;
    }
    bool checkValidationLayerSupport(const std::vector<const char *> &requiredValidationLayers)
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
 
    std::unique_ptr<vk::raii::Instance> createInstance(std::vector<const char *> validationLayers)
    {
        if (!validationLayers.empty() && !checkValidationLayerSupport(validationLayers))
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        vk::raii::Context context;
        uint32_t apiVersion = context.enumerateInstanceVersion();

        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Lava";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        vk::InstanceCreateInfo createInfo{};
        createInfo.pApplicationInfo = &appInfo;
        uint32_t glfwExtensionCount = 0;

        auto requiredExtensions = getRequiredExtensions(!validationLayers.empty());

        createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        auto instance = std::make_unique<vk::raii::Instance>(context, createInfo);
        if (instance == nullptr)
        {
            throw std::runtime_error("failed to create vulkan instance");
        }

        return std::move(instance);
    }


}