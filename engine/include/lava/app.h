#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <lava/core/window.h>
#include <vector>
#include <memory>
#include <optional>
namespace lava
{
    class App
    {
    public:
        App();
        ~App();
        void run();
        void update();
        void render();

    private:
        void init();
        void cleanup();
        void initWindow();
        void initVulkan();
        void createInstance();
        void createDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
        vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

        bool checkDeviceExtensionsSupport(const vk::raii::PhysicalDevice& physicalDevice);
        bool isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice);
        bool checkValidationLayerSupport(const std::vector<const char *> &requiredValidationLayers);
        std::vector<const char *> getRequiredExtensions();

        std::unique_ptr<core::Window> _window;
        std::unique_ptr<vk::raii::Instance> _vulkanInstance;

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);

        std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> _debugMessenger;
        bool _enableValidationLayers;
        std::vector<const char*> _validationLayers;
        std::vector<const char*> _deviceExtensions;

        std::unique_ptr<vk::raii::PhysicalDevice> _physicalDevice;
        std::unique_ptr<vk::raii::Device> _device;
        std::unique_ptr<vk::raii::SurfaceKHR> _surface;
        std::unique_ptr<vk::raii::Queue> _presentQueue;
        std::unique_ptr<vk::raii::SwapchainKHR> _swapchain;
    };
  
}