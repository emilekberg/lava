#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <memory>
#include <vector>
#include <tuple>
#include "lava/rendering/queue-family.hpp"
namespace lava::rendering::constructors
{
    std::tuple<std::unique_ptr<vk::raii::Device>, std::unique_ptr<vk::raii::Queue>, std::unique_ptr<vk::raii::Queue>> createDevice(const vk::raii::Instance &instance, const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR& surface, const std::vector<const char *> &deviceExtensions, std::vector<const char *> &validationLayers)
    {
        rendering::QueueFamilyIndices indices = rendering::findQueueFamilies(physicalDevice, surface);

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
        deviceFeatures.setSamplerAnisotropy(vk::True);

        vk::DeviceCreateInfo createInfo{};
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        auto device = std::make_unique<vk::raii::Device>(std::move(physicalDevice.createDevice(createInfo)));
        auto presentQueue = std::make_unique<vk::raii::Queue>(std::move(device->getQueue(indices.presentFamily.value(), 0)));
        auto graphicsQueue = std::make_unique<vk::raii::Queue>(std::move(device->getQueue(indices.graphicsFamily.value(), 0)));
        
        return std::make_tuple(std::move(device), std::move(presentQueue), std::move(graphicsQueue));
    }
}