#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <memory>
#include <optional>
#include <vector>
#include "lava/rendering/queue-family.hpp"
#include "lava/rendering/swapchain-support-details.hpp"
namespace lava::rendering::constructors
{
    bool checkDeviceExtensionsSupport(const vk::raii::PhysicalDevice &physicalDevice, const std::vector<const char *> &deviceExtensions)
    {
        auto availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

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

    bool isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR& surface, const std::vector<const char *> deviceExtensions)
    {
        auto queueFamilyIndices = rendering::findQueueFamilies(physicalDevice, surface);
        if (!queueFamilyIndices.isComplete())
            return false;
        if (physicalDevice.getProperties().deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
            return false;
        if (!physicalDevice.getFeatures().geometryShader)
            return false;
        if (!checkDeviceExtensionsSupport(physicalDevice, deviceExtensions))
            return false;
        rendering::SwapChainSupportDetails swapChainDetails = rendering::querySwapChainSupport(physicalDevice, surface);
        if (!swapChainDetails.isAdequate())
            return false;
        return true;
    }

    std::unique_ptr<vk::raii::PhysicalDevice> pickPhysicalDevice(const vk::raii::Instance &instance, const vk::raii::SurfaceKHR& surface, const std::vector<const char *> deviceExtensions)
    {
        auto devices = vk::raii::PhysicalDevices(instance);
        if (devices.size() == 0)
        {
            throw std::runtime_error("failed to find GPUs with vulkan support");
        }

        // find suitable device
        std::optional<vk::raii::PhysicalDevice> selectedPhysicalDevice;
        for (const auto &device : devices)
        {
            if (isDeviceSuitable(device, surface, deviceExtensions))
            {
                selectedPhysicalDevice = std::move(device);
                break;
            }
        }
        if (!selectedPhysicalDevice.has_value())
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        auto physicalDevice = std::make_unique<vk::raii::PhysicalDevice>(std::move(selectedPhysicalDevice.value()));
        fprintf(stdout, "Selected device: %s\n", static_cast<const char *>(physicalDevice->getProperties().deviceName));
        return std::move(physicalDevice);
    }

}