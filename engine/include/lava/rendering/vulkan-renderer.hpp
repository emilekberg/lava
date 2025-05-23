#pragma once
#include <vector>
#include <memory>
#include <optional>
#include <vulkan/vulkan_raii.hpp>
#include <queue>
#include "lava/rendering/data/vertex.hpp"
#include "lava/rendering/vulkan-renderer.hpp"
#include "lava/rendering/graphics-pipeline.hpp"
#include "lava/rendering/data/mesh.hpp"
#include "lava/rendering/screensize.hpp"
#include "lava/core/window.hpp"
#include "lava/rendering/data/uniform-buffer-object.hpp"
#include "lava/rendering/renderpass.hpp"
#include "lava/rendering/render-context.hpp"
#include "lava/rendering/data/texture.hpp"
#include "lava/rendering/command.hpp"
#include "lava/rendering/data/vkMesh.hpp"
namespace lava::rendering
{
    class VulkanRenderer
    {
    public:
        VulkanRenderer(const ScreenSize& screenSize, HWND windowHandle);
        
        void preFrame();
        bool render();

        void waitUntilIdle();
        void resize(const ScreenSize& screenSize);
        void requireResize();

    private:
        template< class _Ty, class... _Types, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
        constexpr void queueCommand(CommandType type, _Types&&... args )
        {
            _commandQueue.push({
                .type = type,
                .data = std::make_shared<_Ty>(std::forward<_Types>(args)...),
            });
        }
        // void cleanupSwapChain();
        void recreateSwapChain(const ScreenSize& screenSize);
        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();
        void copyBuffer(const vk::Buffer& sourceBuffer, const vk::Buffer& destinationBuffer, vk::DeviceSize size);
        void createCommandBuffer();
        void createSyncObjects();
        void createDescriptorSetLayout();
        void beginSingleTimeCommands(std::function<void(const vk::raii::CommandBuffer&)> callback);
        void endSingleTimeCommands(const vk::raii::CommandBuffer& commandBuffer);
        void transitionImageLayout(const vk::Image& image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
        void recordCommandBuffer(const vk::raii::CommandBuffer &commandBuffer, uint32_t imageIndex);
        void copyBufferToImage(const vk::raii::Buffer& buffer, const vk::raii::Image& image, uint32_t width, uint32_t height);
        void updateUniformBuffer(uint32_t currentImage);

        void createDepthResource();

        std::tuple<std::unique_ptr<vk::raii::Image>, std::unique_ptr<vk::raii::DeviceMemory>> createImage(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
        std::unique_ptr<data::Texture> createTexture(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice);
        std::unique_ptr<data::VkMesh> createMesh(const std::vector<data::Vertex> vertices, const std::vector<uint32_t> indices);
        std::unique_ptr<vk::raii::ImageView> createTextureImageView();
        void createTextureSampler();


        const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        vk::raii::Instance _vulkanInstance;
        std::unique_ptr<vk::raii::SurfaceKHR> _surface;
        std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> _debugMessenger;
        std::unique_ptr<vk::raii::PhysicalDevice> _physicalDevice;
        std::unique_ptr<vk::raii::Device> _device;
        std::unique_ptr<RenderContext> _renderContext;
        std::unique_ptr<RenderPass> _renderpass;
        std::unique_ptr<vk::raii::Queue> _presentQueue;
        std::unique_ptr<vk::raii::Queue> _graphicsQueue;
        
        std::vector<vk::raii::Semaphore> _imageAvailableSemaphore;
        std::vector<vk::raii::Semaphore> _renderFinishedSemaphore;
        
        std::unique_ptr<vk::raii::DescriptorSetLayout> _descriptorSetLayout;
        std::unique_ptr<rendering::GraphicsPipeline> _graphicsPipeline;
        
        std::unique_ptr<vk::raii::CommandPool> _shortlivedCommandPool;
        std::unique_ptr<vk::raii::CommandPool> _commandPool;
        std::unique_ptr<vk::raii::CommandBuffers> _commandBuffers;
        
        std::vector<vk::raii::Fence> _inFlightFence;
        
        std::unique_ptr<Buffer> _vertexBuffer;
        std::unique_ptr<Buffer> _indexBuffer;
        
        std::unique_ptr<vk::raii::DescriptorPool> _descriptorPool;
        std::unique_ptr<vk::raii::DescriptorSets> _descriptorSets;

        std::vector<std::unique_ptr<Buffer>> _uniformBuffers;
        std::vector<void*> _uniformBuffersMapped;
        
        std::vector<const char *> _validationLayers;
        std::vector<const char *> _deviceExtensions;


        std::unique_ptr<lava::rendering::data::Texture> _texture;
        std::unique_ptr<vk::raii::ImageView> _imageView;
        std::unique_ptr<vk::raii::Sampler> _sampler;
       
        std::unique_ptr<lava::rendering::data::Texture> _depthImage;
        std::unique_ptr<vk::raii::ImageView> _depthImageView; 
    
        std::queue<Command> _commandQueue;

        bool _requiresResize = false;
        uint32_t _currentFrame = 0;

        std::vector<std::unique_ptr<rendering::data::VkMesh>> _meshes;
    };
}