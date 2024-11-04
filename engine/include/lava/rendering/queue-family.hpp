#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <optional>
namespace lava::rendering
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    }; 
    
    QueueFamilyIndices findQueueFamilies(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface)
    {
        QueueFamilyIndices indices;

        int i = 0;
    
        auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
        for(const auto& queueFamily : queueFamilyProperties)
        {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                indices.graphicsFamily = i;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, i, *surface, &presentSupport); 
            if(presentSupport)
            {
                indices.presentFamily = i;
            }
            i++;
        }

        return indices;
    }

}