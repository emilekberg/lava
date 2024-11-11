#include "lava/rendering/vulkan-renderer.hpp"
#include "lava/rendering/builders/graphics-pipeline-builder.hpp"
#include "lava/rendering/constructors/instance.hpp"
#include "lava/rendering/constructors/debug-messenger.hpp"
#include "lava/rendering/constructors/surface.hpp"
#include "lava/rendering/constructors/physical-device.hpp"
#include "lava/rendering/constructors/device.hpp"
#include "lava/rendering/constructors/swapchain.hpp"
#include "lava/rendering/constructors/image-views.hpp"
// #include "lava/rendering/constructors/_renderpass.hpp"
#include "lava/rendering/constructors/framebuffers.hpp"
#include "lava/rendering/constructors/commandpool.hpp"
#include "lava/rendering/constructors/buffer.hpp"
#include "lava/core/window.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "lava/resourceloader.hpp"
#define UNHANDLED_PARAMETER(param) param;
#undef max
namespace lava::rendering
{
    VulkanRenderer::VulkanRenderer(const ScreenSize &screenSize, HWND windowHandle) :  _validationLayers({"VK_LAYER_KHRONOS_validation"}),
                                                                                      _deviceExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME}),
                                                                                      _vulkanInstance(VK_NULL_HANDLE)
    {
        _mesh = lava::resourceloader::loadMesh("./build/mesh/viking_room.obj");
        _vulkanInstance = constructors::createInstance(_validationLayers);
        _debugMessenger = constructors::createDebugMessenger(_vulkanInstance);
        _surface = constructors::createSurface(_vulkanInstance, windowHandle);
        _physicalDevice = constructors::pickPhysicalDevice(_vulkanInstance, *_surface.get(), _deviceExtensions);
        std::tie(_device, _presentQueue, _graphicsQueue) = constructors::createDevice(_vulkanInstance, *_physicalDevice.get(), *_surface.get(), _deviceExtensions, _validationLayers);

        _renderContext = std::make_unique<RenderContext>(*_device.get(), *_surface.get(), *_physicalDevice.get(), screenSize);
        _renderpass = std::make_unique<RenderPass>(*_device.get(), *_physicalDevice.get(), *_renderContext.get());
        createDescriptorSetLayout();
        _graphicsPipeline = builders::GraphicsPipelineBuilder(*_device.get())
                                .withFragmentShader("./build/shaders/shader_frag.spv")
                                .withVertexShader("./build/shaders/shader_vert.spv")
                                .withVertexInputInfo(data::Vertex::getBindingDescription(), data::Vertex::getAttributeDescriptions())
                                .withExtent(_renderContext->getExtent())
                                // .withRenderPass(_renderContext->getRenderpass())
                                .build(*_descriptorSetLayout.get(), _renderpass->getRenderpass());
        _commandPool = constructors::createCommandPool(*_device.get(), *_physicalDevice.get(), *_surface.get());
        _shortlivedCommandPool = constructors::createTransientCommandPool(*_device.get(), *_physicalDevice.get(), *_surface.get());

        createVertexBuffers();
        createIndexBuffers();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffer();
        createSyncObjects();

    }

    VulkanRenderer::~VulkanRenderer()
    {
        //cleanupSwapChain();
    }

    void VulkanRenderer::cleanupSwapChain()
    {
        //_swapchainFrameBuffers.clear();
        //_swapchainImageViews.clear();
        //_swapchain = nullptr;
    }
    void VulkanRenderer::recreateSwapChain(const ScreenSize &screenSize)
    {
        _device->waitIdle();
        _renderContext->resize(screenSize);
        cleanupSwapChain();
    }

    void VulkanRenderer::copyBuffer(const vk::raii::Buffer &sourceBuffer, const vk::raii::Buffer &destinationBuffer, vk::DeviceSize size)
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

        std::tie(stagingBuffer, stagingBufferMemory) = constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        void *data = stagingBufferMemory->mapMemory(0, bufferSize);
        memcpy(data, _mesh.vertices.data(), (size_t)bufferSize);
        stagingBufferMemory->unmapMemory();

        std::tie(_vertexBuffer, _vertexBufferMemory) = constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
        copyBuffer(*stagingBuffer.get(), *_vertexBuffer.get(), bufferSize);
    }

    void VulkanRenderer::createIndexBuffers()
    {
        vk::DeviceSize bufferSize = sizeof(_mesh.indices[0]) * _mesh.indices.size();
        std::unique_ptr<vk::raii::Buffer> stagingBuffer;
        std::unique_ptr<vk::raii::DeviceMemory> stagingBufferMemory;

        std::tie(stagingBuffer, stagingBufferMemory) = constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        void *data = stagingBufferMemory->mapMemory(0, bufferSize);
        memcpy(data, _mesh.indices.data(), (size_t)bufferSize);
        stagingBufferMemory->unmapMemory();

        std::tie(_indexBuffer, _indexBufferMemory) = constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
        copyBuffer(*stagingBuffer.get(), *_indexBuffer.get(), bufferSize);
    }

    void VulkanRenderer::createUniformBuffers()
    {
        vk::DeviceSize bufferSize = sizeof(data::UniformBufferObject);
        _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        _uniformBufferMemories.resize(MAX_FRAMES_IN_FLIGHT);
        _uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            std::tie(_uniformBuffers[i], _uniformBufferMemories[i]) = constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
            _uniformBuffersMapped[i] = _uniformBufferMemories[i]->mapMemory(0, bufferSize);
        }
    }

    void VulkanRenderer::createDescriptorPool()
    {
        vk::DescriptorPoolSize poolSize{};
        poolSize.setDescriptorCount(static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo
            .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setPoolSizeCount(1)
            .setPoolSizes(poolSize)
            .setMaxSets(static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));

        _descriptorPool = std::make_unique<vk::raii::DescriptorPool>(*_device.get(), poolInfo);
    }

    void VulkanRenderer::createDescriptorSets()
    {
        std::vector<vk::DescriptorSetLayout> layouts;
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            layouts.push_back(*_descriptorSetLayout.get());
        }
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo
            .setDescriptorPool(*_descriptorPool.get())
            .setDescriptorSetCount(MAX_FRAMES_IN_FLIGHT)
            .setPSetLayouts(layouts.data());

        _descriptorSets = std::make_unique<vk::raii::DescriptorSets>(*_device.get(), allocInfo);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vk::DescriptorBufferInfo bufferInfo{};
            bufferInfo
                .setBuffer(*_uniformBuffers[i].get())
                .setOffset(0)
                .setRange(sizeof(data::UniformBufferObject));

            vk::WriteDescriptorSet descriptorWrite{};
            descriptorWrite
                .setDstSet(_descriptorSets->at(i))
                .setDstBinding(0)
                .setDstArrayElement(0)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDescriptorCount(1)
                .setPBufferInfo(&bufferInfo)
                .setPImageInfo(nullptr)
                .setPTexelBufferView(nullptr);

            _device->updateDescriptorSets(descriptorWrite, 0);
        }
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
    void VulkanRenderer::createDescriptorSetLayout()
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding
            .setBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eVertex)
            .setPImmutableSamplers(nullptr);

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo
            .setBindingCount(1)
            .setPBindings(&uboLayoutBinding);

        _descriptorSetLayout = std::make_unique<vk::raii::DescriptorSetLayout>(*_device.get(), layoutInfo);
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
            std::tie(result, imageIndex) = _renderContext->getSwapchain().acquireNextImage(UINT64_MAX, *_imageAvailableSemaphore[_currentFrame]);
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
            return true;
        }
        _device->resetFences({*_inFlightFence[_currentFrame]});
        const vk::raii::CommandBuffer& commandBuffer = _commandBuffers->at(_currentFrame);
        commandBuffer.reset();

        recordCommandBuffer(commandBuffer, imageIndex);

        updateUniformBuffer(_currentFrame);

        vk::SubmitInfo submitInfo = {};

        vk::Semaphore waitSemaphore[] = {*_imageAvailableSemaphore[_currentFrame]};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.setWaitSemaphoreCount(1);
        submitInfo.setPWaitSemaphores(waitSemaphore);
        submitInfo.setPWaitDstStageMask(waitStages);

        vk::CommandBuffer commandBuffers[] = {commandBuffer};
        submitInfo.setCommandBufferCount(1);
        submitInfo.setCommandBuffers(commandBuffers);

        vk::Semaphore signalSemaphores[] = {*_renderFinishedSemaphore[_currentFrame]};
        submitInfo.setSignalSemaphoreCount(1);
        submitInfo.setPSignalSemaphores(signalSemaphores);

        _graphicsQueue->submit({submitInfo}, *_inFlightFence[_currentFrame]);

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphoreCount(1);
        presentInfo.setPWaitSemaphores(signalSemaphores);

        vk::SwapchainKHR swapChains[] = {_renderContext->getSwapchain()};
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


        _renderpass->begin(commandBuffer, *_renderContext.get(), imageIndex);
        {
            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _graphicsPipeline->getVkPipeline());

            vk::Viewport viewport{};
            viewport
                .setX(0.0f)
                .setY(0.0f)
                .setWidth(static_cast<float>(_renderContext->getExtent().width))
                .setHeight(static_cast<float>(_renderContext->getExtent().height))
                .setMinDepth(0.0f)
                .setMaxDepth(0.0f);
            commandBuffer.setViewport(0, viewport);

            vk::Rect2D scissor{};
            scissor.setOffset({0, 0})
                .setExtent(_renderContext->getExtent());
            commandBuffer.setScissor(0, scissor);

            vk::Buffer vertexBuffers[] = {*_vertexBuffer.get()};
            vk::DeviceSize offsets[] = {0};
            commandBuffer.bindVertexBuffers(0, vertexBuffers, offsets);
            commandBuffer.bindIndexBuffer(*_indexBuffer.get(), {0}, vk::IndexType::eUint32);
            
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _graphicsPipeline->getVkPipelineLayout(), 0, {_descriptorSets->at(_currentFrame)}, nullptr);
            commandBuffer.drawIndexed(static_cast<uint32_t>(_mesh.indices.size()), 1, 0, 0, 0);
        }
        _renderpass->end(commandBuffer);
        commandBuffer.end();
    }

    void VulkanRenderer::updateUniformBuffer(uint32_t currentImage)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        data::UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.projection = glm::perspective(45.0f, _renderContext->getExtent().width / (float)_renderContext->getExtent().height, 0.1f, 10.0f);
        ubo.projection[1][1] *= -1;

        memcpy(_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }

    void VulkanRenderer::waitUntilIdle()
    {
        _device->waitIdle();
    }
    void VulkanRenderer::resize(const ScreenSize &screenSize)
    {
        _requiresResize = false;
        recreateSwapChain(screenSize);
        _renderpass->recreateFramebuffers(*_renderContext.get());
    }
}