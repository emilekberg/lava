#include "lava/rendering/vulkan-renderer.hpp"
#include "lava/rendering/builders/graphics-pipeline-builder.hpp"
#include "lava/rendering/shader.hpp"
#include "lava/rendering/attribute-description-builder.hpp"
#include "lava/rendering/constructors/instance.hpp"
#include "lava/rendering/constructors/debug-messenger.hpp"
#include "lava/rendering/constructors/surface.hpp"
#include "lava/rendering/constructors/physical-device.hpp"
#include "lava/rendering/constructors/device.hpp"
#include "lava/rendering/constructors/swapchain.hpp"
#include "lava/rendering/constructors/image-views.hpp"
#include "lava/rendering/constructors/renderpass.hpp"
#include "lava/rendering/constructors/framebuffers.hpp"
#include "lava/rendering/constructors/commandpool.hpp"
#include "lava/rendering/constructors/buffer.hpp"
#include "lava/core/window.hpp"

#define UNHANDLED_PARAMETER(param) param;
#undef max
namespace lava::rendering
{
    VulkanRenderer::VulkanRenderer(const ScreenSize& screenSize, HWND windowHandle) :
        _validationLayers({"VK_LAYER_KHRONOS_validation"}),
        _deviceExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
    {
        _vulkanInstance = constructors::createInstance(_validationLayers);
        _debugMessenger = constructors::createDebugMessenger(*_vulkanInstance.get());
        _surface = constructors::createSurface(*_vulkanInstance.get(), windowHandle);
        _physicalDevice = constructors::pickPhysicalDevice(*_vulkanInstance.get(), *_surface.get(), _deviceExtensions);

        std::tie(_device, _presentQueue, _graphicsQueue) = constructors::createDevice(*_vulkanInstance.get(), *_physicalDevice.get(), *_surface.get(), _deviceExtensions, _validationLayers);

        std::tie(_swapchain, _swapchainImageFormat, _swapchainExtent) = constructors::createSwapChain(*_device.get(), *_physicalDevice.get(), *_surface.get(), screenSize);
        _swapchainImages = _swapchain->getImages();
        _swapchainImageViews = constructors::createImageViews(*_device.get(), _swapchainImages, _swapchainImageFormat);
        _renderpass = constructors::createRenderPass(*_device.get(), _swapchainImageFormat);
        _graphicsPipeline = builders::GraphicsPipelineBuilder(*_device.get())
                                .withFragmentShader("./build/shaders/shader_frag.spv")
                                .withVertexShader("./build/shaders/shader_vert.spv")
                                .withVertexInputInfo(Vertex::getBindingDescription(), Vertex::getAttributeDescriptions())
                                .withExtent(_swapchainExtent)
                                .withRenderPass(_renderpass)
                                .build();
        _swapchainFrameBuffers = constructors::createFrameBuffers(*_device.get(), *_renderpass.get(), _swapchainExtent, _swapchainImageViews);
        _commandPool = constructors::createCommandPool(*_device.get(), *_physicalDevice.get(), *_surface.get());
        _shortlivedCommandPool = constructors::createTransientCommandPool(*_device.get(), *_physicalDevice.get(), *_surface.get());

        createVertexBuffers();
        createIndexBuffers();
        createCommandBuffer();
        createSyncObjects();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        cleanupSwapChain();
        _vertexBuffer = nullptr;
        _vertexBufferMemory = nullptr;
        _indexBuffer = nullptr;
        _indexBufferMemory = nullptr;
        _inFlightFence.clear();
        _commandBuffers->clear();
        _commandPool = nullptr;
        _shortlivedCommandPool = nullptr;
        _graphicsPipeline = nullptr;
        _renderpass = nullptr;

        _renderFinishedSemaphore.clear();
        _imageAvailableSemaphore.clear();

        _swapchainImages.clear();
        _swapchain = nullptr;
        _presentQueue = nullptr;
        _graphicsQueue = nullptr;
        _device = nullptr;
        _physicalDevice = nullptr;
        // _window = nullptr;
        _debugMessenger = nullptr;
        _surface = nullptr;
        _vulkanInstance = nullptr;
    }

    void VulkanRenderer::cleanupSwapChain()
    {
        _swapchainFrameBuffers.clear();
        _swapchainImageViews.clear();
        _swapchain = nullptr;
    }
    void VulkanRenderer::recreateSwapChain(const ScreenSize &screenSize)
    {
        _device->waitIdle();
        cleanupSwapChain();
        std::unique_ptr<vk::raii::SwapchainKHR> newSwapchain;
        std::tie(newSwapchain, _swapchainImageFormat, _swapchainExtent) = constructors::createSwapChain(*_device.get(), *_physicalDevice.get(), *_surface.get(), screenSize);

        _swapchain = std::move(newSwapchain);
        _swapchainImages = _swapchain->getImages();

        _swapchainImageViews = constructors::createImageViews(*_device.get(), _swapchainImages, _swapchainImageFormat);
        _swapchainFrameBuffers = constructors::createFrameBuffers(*_device.get(), *_renderpass.get(), _swapchainExtent, _swapchainImageViews);
    }

    void VulkanRenderer::copyBuffer(const vk::raii::Buffer& sourceBuffer, const vk::raii::Buffer& destinationBuffer, vk::DeviceSize size)
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandPool(*_commandPool.get())
            .setCommandBufferCount(1);

        vk::raii::CommandBuffers commandBuffers(*_device.get(), allocInfo);
        const vk::raii::CommandBuffer commandBuffer = std::move(commandBuffers[0]);

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commandBuffer.begin(beginInfo);
        vk::BufferCopy copyRegion;

        copyRegion
            .setSrcOffset(0)
            .setDstOffset(0)
            .setSize(size);
        commandBuffer.copyBuffer(sourceBuffer, destinationBuffer, copyRegion);
        commandBuffer.end();

        vk::CommandBuffer tmpCommandBuffers[] = {commandBuffer};

        vk::SubmitInfo submitInfo{};
        submitInfo
            .setCommandBufferCount(1)
            .setCommandBuffers(tmpCommandBuffers);
        
        _graphicsQueue->submit(submitInfo, VK_NULL_HANDLE);
        _graphicsQueue->waitIdle();
        
    }


    void VulkanRenderer::createVertexBuffers()
    {
        vk::DeviceSize bufferSize = sizeof(_mesh.vertices[0]) * _mesh.vertices.size();
        std::unique_ptr<vk::raii::Buffer> stagingBuffer;
        std::unique_ptr<vk::raii::DeviceMemory> stagingBufferMemory;

        std::tie(stagingBuffer, stagingBufferMemory) = lava::rendering::constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        
        void *data = stagingBufferMemory->mapMemory(0, bufferSize);
        memcpy(data, _mesh.vertices.data(), (size_t)bufferSize);
        stagingBufferMemory->unmapMemory();

        std::tie(_vertexBuffer, _vertexBufferMemory) = lava::rendering::constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
        copyBuffer(*stagingBuffer.get(), *_vertexBuffer.get(), bufferSize);
    }

    void VulkanRenderer::createIndexBuffers()
    {
        vk::DeviceSize bufferSize = sizeof(_mesh.indices[0]) * _mesh.indices.size();
        std::unique_ptr<vk::raii::Buffer> stagingBuffer;
        std::unique_ptr<vk::raii::DeviceMemory> stagingBufferMemory;

        std::tie(stagingBuffer, stagingBufferMemory) = lava::rendering::constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        
        void *data = stagingBufferMemory->mapMemory(0, bufferSize);
        memcpy(data, _mesh.indices.data(), (size_t)bufferSize);
        stagingBufferMemory->unmapMemory();

        std::tie(_indexBuffer, _indexBufferMemory) = lava::rendering::constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
        copyBuffer(*stagingBuffer.get(), *_indexBuffer.get(), bufferSize);
    }

    void VulkanRenderer::createCommandBuffer()
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setCommandPool(*_commandPool.get());
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        allocInfo.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);

        _commandBuffers = std::make_unique<vk::raii::CommandBuffers>(*_device.get(), allocInfo);
    }

    void VulkanRenderer::createSyncObjects()
    {
        vk::SemaphoreCreateInfo semaphoreInfo{};
        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            _imageAvailableSemaphore.push_back(vk::raii::Semaphore(*_device.get(), semaphoreInfo));
            _renderFinishedSemaphore.push_back(vk::raii::Semaphore(*_device.get(), semaphoreInfo));
            _inFlightFence.push_back(vk::raii::Fence(*_device.get(), fenceInfo));
        }
    }

    void VulkanRenderer::requireResize()
    {
        _requiresResize = true;
    }

    bool VulkanRenderer::render()
    {
        vk::Result result = _device->waitForFences({*_inFlightFence[_currentFrame]}, vk::True, UINT64_MAX);

        uint32_t imageIndex;

#ifndef VULKAN_HPP_NO_EXCEPTIONS
        try
#endif
        {
            std::tie(result, imageIndex) = _swapchain->acquireNextImage(UINT64_MAX, *_imageAvailableSemaphore[_currentFrame]);
            if (result == vk::Result::eErrorOutOfDateKHR)
            {
                _requiresResize = true;
            }
            else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
            {
                throw std::runtime_error("failed to acquire swap chain image!");
            }
        }
#ifndef VULKAN_HPP_NO_EXCEPTIONS
        catch (vk::OutOfDateKHRError &e)
        {
            UNHANDLED_PARAMETER(e)
            _requiresResize = true;
        }
#endif
        if (_requiresResize)
        {
            _requiresResize = false;
            return true;
        }
        _device->resetFences({*_inFlightFence[_currentFrame]});

        _commandBuffers->at(_currentFrame).reset();

        recordCommandBuffer(_commandBuffers->at(_currentFrame), imageIndex);

        vk::SubmitInfo submitInfo = {};

        vk::Semaphore waitSemaphore[] = {*_imageAvailableSemaphore[_currentFrame]};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.setWaitSemaphoreCount(1);
        submitInfo.setPWaitSemaphores(waitSemaphore);
        submitInfo.setPWaitDstStageMask(waitStages);

        vk::CommandBuffer commandBuffers[] = {_commandBuffers->at(_currentFrame)};
        submitInfo.setCommandBufferCount(1);
        submitInfo.setCommandBuffers(commandBuffers);

        vk::Semaphore signalSemaphores[] = {*_renderFinishedSemaphore[_currentFrame]};
        submitInfo.setSignalSemaphoreCount(1);
        submitInfo.setPSignalSemaphores(signalSemaphores);

        _graphicsQueue->submit({submitInfo}, *_inFlightFence[_currentFrame]);

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphoreCount(1);
        presentInfo.setPWaitSemaphores(signalSemaphores);

        vk::SwapchainKHR swapChains[] = {*_swapchain.get()};
        presentInfo.setSwapchainCount(1);
        presentInfo.setPSwapchains(swapChains);
        presentInfo.setPImageIndices(&imageIndex);

#ifndef VULKAN_HPP_NO_EXCEPTIONS
        try
#endif
        {

            result = _presentQueue->presentKHR(presentInfo);
            if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
            {
                _requiresResize = true;
            }
            else if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error("failed to present swap chain image!");
            }
        }
#ifndef VULKAN_HPP_NO_EXCEPTIONS
        catch (const vk::OutOfDateKHRError &e)
        {
            UNHANDLED_PARAMETER(e)
            _requiresResize = true;
        }
#endif
        _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        if (_requiresResize)
        {
            _requiresResize = false;
            return true;
        }
        return false;
    }
    void VulkanRenderer::recordCommandBuffer(const vk::raii::CommandBuffer &commandBuffer, uint32_t imageIndex)
    {
        vk::CommandBufferBeginInfo beginInfo{};
        // beginInfo.setFlags();
        // beginInfo.setPInheritanceInfo(nullptr);

        commandBuffer.begin(beginInfo);

        vk::RenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.setRenderPass(*_renderpass.get());
        renderPassBeginInfo.setFramebuffer(_swapchainFrameBuffers[imageIndex]);
        renderPassBeginInfo.setRenderArea({0, 0});
        renderPassBeginInfo.renderArea.setExtent(_swapchainExtent);

        vk::ClearValue clearColor = {{0.0f, 0.0f, 0.0f, 1.0f}};
        renderPassBeginInfo.setClearValueCount(1);
        renderPassBeginInfo.setPClearValues(&clearColor);

        commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _graphicsPipeline->getVkPipeline());

        vk::Viewport viewport{};
        viewport.setX(0.0f);
        viewport.setY(0.0f);
        viewport.setWidth(static_cast<float>(_swapchainExtent.width));
        viewport.setHeight(static_cast<float>(_swapchainExtent.height));
        viewport.setMinDepth(0.0f);
        viewport.setMaxDepth(0.0f);
        commandBuffer.setViewport(0, viewport);

        vk::Rect2D scissor{};
        scissor.setOffset({0, 0});
        scissor.setExtent(_swapchainExtent);
        commandBuffer.setScissor(0, scissor);

        vk::Buffer vertexBuffers[] = {*_vertexBuffer.get()};
        vk::Buffer indexBuffers[] = {*_indexBuffer.get()};
        vk::DeviceSize offsets[] = {0};
        commandBuffer.bindVertexBuffers(0, vertexBuffers, offsets);
        commandBuffer.bindIndexBuffer(*_indexBuffer.get(), {0}, vk::IndexType::eUint32);

        commandBuffer.drawIndexed(static_cast<uint32_t>(_mesh.indices.size()), 1, 0, 0, 0);
        // commandBuffer.draw(static_cast<uint32_t>(_mesh.vertices.size()), 1, 0, 0);
        // commandBuffer.draw(3, 1, 0, 0);

        commandBuffer.endRenderPass();
        commandBuffer.end();
    }

    void VulkanRenderer::waitUntilIdle()
    {
        _device->waitIdle();
    }
    void VulkanRenderer::resize(const ScreenSize& screenSize)
    {
        recreateSwapChain(screenSize);
    }
}