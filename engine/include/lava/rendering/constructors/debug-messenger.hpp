#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <memory>
namespace lava::rendering::constructors
{
    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
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

    std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> createDebugMessenger(const vk::raii::Instance& instance)
    {
        vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
        createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional

        auto debugMessenger = std::make_unique<vk::raii::DebugUtilsMessengerEXT>(instance, createInfo, nullptr);
        if (debugMessenger == nullptr)
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
        return std::move(debugMessenger);
    }
    }