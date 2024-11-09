#pragma once
#include <vector>
#include <memory>
#include <optional>
#include <vulkan/vulkan_raii.hpp>
#include "lava/rendering/vertex.hpp"
#include "lava/rendering/vulkan-renderer.hpp"
#include "lava/rendering/graphics-pipeline.hpp"
#include "lava/rendering/mesh.hpp"
#include "lava/rendering/screensize.hpp"
#include "lava/core/window.hpp"

namespace lava::rendering
{
    class VulkanRenderer
    {
    public:
        VulkanRenderer(const ScreenSize& screenSize, HWND windowHandle);
        ~VulkanRenderer();
        
        bool render();

        void waitUntilIdle();
        void resize(const ScreenSize& screenSize);
        void requireResize();

    private:
        void cleanupSwapChain();
        void recreateSwapChain(const ScreenSize& screenSize);
        void createVertexBuffers();
        void createIndexBuffers();
        void copyBuffer(const vk::raii::Buffer& sourceBuffer, const vk::raii::Buffer& destinationBuffer, vk::DeviceSize size);
        void createCommandBuffer();
        void createSyncObjects();
        void recordCommandBuffer(const vk::raii::CommandBuffer &commandBuffer, uint32_t imageIndex);

        const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

        std::unique_ptr<vk::raii::Instance> _vulkanInstance;

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
        std::unique_ptr<vk::raii::CommandPool> _shortlivedCommandPool;


        std::vector<vk::raii::Semaphore> _imageAvailableSemaphore;
        std::vector<vk::raii::Semaphore> _renderFinishedSemaphore;
        std::vector<vk::raii::Fence> _inFlightFence;
        bool _requiresResize = false;
        uint32_t _currentFrame = 0;

        std::unique_ptr<vk::raii::DeviceMemory> _vertexBufferMemory;
        std::unique_ptr<vk::raii::Buffer> _vertexBuffer;
        std::unique_ptr<vk::raii::DeviceMemory> _indexBufferMemory;
        std::unique_ptr<vk::raii::Buffer> _indexBuffer;

        rendering::Mesh _mesh;
        std::unique_ptr<rendering::GraphicsPipeline> _graphicsPipeline;
    };
}