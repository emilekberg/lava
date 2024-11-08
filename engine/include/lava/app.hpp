#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <vulkan/vulkan_raii.hpp>
#include "lava/core/window.hpp"
#include "lava/rendering/vertex.hpp"
#include "lava/rendering/vulkan-renderer.hpp"
#include "lava/rendering/graphics-pipeline.hpp"
#include "lava/rendering/mesh.hpp"
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
        void recreateSwapChain();
        void createRenderPass();
        void createGraphicsPipeline();
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

        const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

        std::unique_ptr<core::Window> _window;
        std::unique_ptr<vk::raii::Instance> _vulkanInstance;

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);

        static void handleWindowResize(GLFWwindow* window, int width, int height);

        std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> _debugMessenger;
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

        std::shared_ptr<vk::raii::RenderPass> _renderpass;
        // std::unique_ptr<vk::raii::PipelineLayout> _pipelineLayout;
        // std::unique_ptr<vk::raii::Pipeline> _pipeline;

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

        rendering::Mesh _mesh;
        std::unique_ptr<rendering::GraphicsPipeline> _graphicsPipeline;
    };

}