#pragma once

#include <vulkan/vulkan_raii.hpp>
#include "lava/core/window.h"
#include <vector>
#include <memory>
#include <optional>
#include "lava/data/vertex.h"
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
        void cleanupSwapChain();
        void cleanup();
        void initWindow();
        void initVulkan();
        void createInstance();
        void createDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void recreateSwapChain();
        void createImageViews();
        void createRenderPass();
        void createGraphicsPipeline();
        void createFrameBuffers();
        void createCommandPool();
        void createVertexBuffers();
        void createCommandBuffer();
        void createSyncObjects();
        void recordCommandBuffer(const vk::raii::CommandBuffer &commandBuffer, uint32_t imageIndex);
        void setFrameBufferResized();

        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

        vk::raii::ShaderModule createShaderModule(const std::vector<char> &code);

        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
        vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);

        bool checkDeviceExtensionsSupport(const vk::raii::PhysicalDevice &physicalDevice);
        bool isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice);
        bool checkValidationLayerSupport(const std::vector<const char *> &requiredValidationLayers);
        std::vector<const char *> getRequiredExtensions();
        static VKAPI_ATTR vk::Bool32 VKAPI_CALL vkdebugCallback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            vk::DebugUtilsMessageTypeFlagsEXT messageType,
            const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);

        const size_t MAX_FRAMES_IN_FLIGHT = 2;

        std::unique_ptr<core::Window> _window;
        std::unique_ptr<vk::raii::Instance> _vulkanInstance;

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);

        static void handleWindowResize(GLFWwindow* window, int width, int height);

        std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> _debugMessenger;
        bool _enableValidationLayers;
        std::vector<const char *> _validationLayers;
        std::vector<const char *> _deviceExtensions;

        std::unique_ptr<vk::raii::PhysicalDevice> _physicalDevice;
        std::unique_ptr<vk::raii::Device> _device;
        std::unique_ptr<vk::raii::SurfaceKHR> _surface;
        std::unique_ptr<vk::raii::Queue> _presentQueue;
        std::unique_ptr<vk::raii::Queue> _graphicsQueue;

        std::unique_ptr<vk::raii::SwapchainKHR> _swapchain;
        std::vector<vk::Image> _swapchainImages;
        vk::Format _swapchainImageFormat;
        vk::Extent2D _swapchainExtent;

        std::vector<vk::raii::ImageView> _swapchainImageViews;

        std::unique_ptr<vk::raii::RenderPass> _renderpass;
        std::unique_ptr<vk::raii::PipelineLayout> _pipelineLayout;
        std::unique_ptr<vk::raii::Pipeline> _pipeline;

        std::vector<vk::raii::Framebuffer> _swapchainFrameBuffers;

        std::unique_ptr<vk::raii::CommandPool> _commandPool;
        std::unique_ptr<vk::raii::CommandBuffers> _commandBuffers;

        std::vector<vk::raii::Semaphore> _imageAvailableSemaphore;
        std::vector<vk::raii::Semaphore> _renderFinishedSemaphore;
        std::vector<vk::raii::Fence> _inFlightFence;
        bool _framebufferResized = false;
        uint32_t _currentFrame = 0;

        std::unique_ptr<vk::raii::DeviceMemory> _vertexBufferMemory;
        std::unique_ptr<vk::raii::Buffer> _vertexBuffer;
        std::vector<data::Vertex> _vertices;
    };

}