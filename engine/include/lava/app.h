#pragma once
#include <glfw/glfw3.h>
#include <vulkan/vulkan_raii.hpp>
#include <lava/core/window.h>
#include <vector>
#include <memory>
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
        void createDebugMessenger();
        void createInstance();    
        bool checkValidationLayerSupport(const std::vector<const char*>& requiredValidationLayers);
        std::vector<const char*> getRequiredExtensions();

        std::unique_ptr<core::Window> _window;
        std::unique_ptr<vk::raii::Instance> _vulkanInstance;

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		    void* pUserData);

        std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> _debugMessenger;
        bool _enableValidationLayers;
    };
}