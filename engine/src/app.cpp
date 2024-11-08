#include "lava/app.hpp"
#include <set>
#include "lava/rendering/queue-family.hpp"
#include "lava/rendering/swapchain-support-details.hpp"
#include "lava/resourceloader.hpp"
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

#define UNHANDLED_PARAMETER(param) param;
#undef max
namespace lava
{
    App::App()
    {
        _validationLayers = {
            "VK_LAYER_KHRONOS_validation"};
        _deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    }
    App::~App()
    {
        cleanup();
    }
    void App::init()
    {
        initWindow();
        initVulkan();
    }

    void App::initWindow()
    {
        _window = std::make_unique<core::Window>();
        glfwSetWindowUserPointer(_window->getGLFWwindow(), this);
        _window->setResizeHandler(handleWindowResize);
        _window->activate();
    }

    void App::initVulkan()
    {
        _vulkanInstance = rendering::constructors::createInstance(_validationLayers);
        _debugMessenger = rendering::constructors::createDebugMessenger(*_vulkanInstance.get());
        _surface = rendering::constructors::createSurface(*_vulkanInstance.get(), _window->getWindowHandle());
        _physicalDevice = rendering::constructors::pickPhysicalDevice(*_vulkanInstance.get(), *_surface.get(), _deviceExtensions);

        std::tie(_device, _presentQueue, _graphicsQueue) = rendering::constructors::createDevice(*_vulkanInstance.get(), *_physicalDevice.get(), *_surface.get(), _deviceExtensions, _validationLayers);

        std::tie(_swapchain, _swapchainImageFormat, _swapchainExtent) = rendering::constructors::createSwapChain(*_device.get(), *_physicalDevice.get(), *_surface.get(), *_window.get());
        _swapchainImages = _swapchain->getImages();
        _swapchainImageViews = rendering::constructors::createImageViews(*_device.get(), _swapchainImages, _swapchainImageFormat);
        _renderpass = rendering::constructors::createRenderPass(*_device.get(), _swapchainImageFormat);
        _graphicsPipeline = rendering::builders::GraphicsPipelineBuilder(*_device.get())
                                .withFragmentShader()
                                .withVertexShader()
                                .withVertexInputInfo()
                                .withExtent(_swapchainExtent)
                                .withRenderPass(_renderpass)
                                .build();
        _swapchainFrameBuffers = rendering::constructors::createFrameBuffers(*_device.get(), *_renderpass.get(), _swapchainExtent, _swapchainImageViews);
        _commandPool = rendering::constructors::createCommandPool(*_device.get(), *_physicalDevice.get(), *_surface.get());
        createVertexBuffers();
        createCommandBuffer();
        createSyncObjects();
    }

    void App::cleanupSwapChain()
    {
        _swapchainFrameBuffers.clear();
        _swapchainImageViews.clear();
        _swapchain = nullptr;
    }

    void App::cleanup()
    {
        cleanupSwapChain();
        _vertexBuffer = nullptr;
        _vertexBufferMemory = nullptr;
        _inFlightFence.clear();
        _commandBuffers->clear();
        _commandPool = nullptr;
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
        _window = nullptr;
        _debugMessenger = nullptr;
        _surface = nullptr;
        _vulkanInstance = nullptr;
    }
    void App::recreateSwapChain()
    {
        // handle case where the window is minimized. the width and height is 0 in this case.
        int width = 0, height = 0;
        std::tie(width, height) = _window->getSize();
        while (width == 0 || height == 0)
        {
            std::tie(width, height) = _window->getSize();
            _window->pollEvents();
        }

        _device->waitIdle();
        cleanupSwapChain();

        std::tie(_swapchain, _swapchainImageFormat, _swapchainExtent) = rendering::constructors::createSwapChain(*_device.get(), *_physicalDevice.get(), *_surface.get(), *_window.get());
        _swapchainImages = _swapchain->getImages();

        _swapchainImageViews = rendering::constructors::createImageViews(*_device.get(), _swapchainImages, _swapchainImageFormat);
        _swapchainFrameBuffers = rendering::constructors::createFrameBuffers(*_device.get(), *_renderpass.get(), _swapchainExtent, _swapchainImageViews);
    }

    vk::raii::ShaderModule App::createShaderModule(const std::vector<char> &code)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.setCodeSize(code.size());
        createInfo.setPCode(reinterpret_cast<const uint32_t *>(code.data()));
        return vk::raii::ShaderModule(*_device.get(), createInfo);
    }

    void App::createVertexBuffers()
    {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(sizeof(_mesh.vertices[0]) * _mesh.vertices.size());
        bufferInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
        bufferInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
        bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

        _vertexBuffer = std::make_unique<vk::raii::Buffer>(*_device.get(), bufferInfo);

        auto memoryRequirements = _vertexBuffer->getMemoryRequirements();

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.setAllocationSize(memoryRequirements.size);
        allocInfo.setMemoryTypeIndex(findMemoryType(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

        _vertexBufferMemory = std::make_unique<vk::raii::DeviceMemory>(*_device.get(), allocInfo);
        _vertexBuffer->bindMemory(*_vertexBufferMemory.get(), 0);

        void *data = _vertexBufferMemory->mapMemory(0, bufferInfo.size);
        memcpy(data, _mesh.vertices.data(), (size_t)bufferInfo.size);
        _vertexBufferMemory->unmapMemory();
    }

    void App::createCommandBuffer()
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setCommandPool(*_commandPool.get());
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        allocInfo.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);

        _commandBuffers = std::make_unique<vk::raii::CommandBuffers>(*_device.get(), allocInfo);
    }

    void App::createSyncObjects()
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

    void App::setFrameBufferResized()
    {
        _framebufferResized = true;
    }

    uint32_t App::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memoryProperties = _physicalDevice->getMemoryProperties();
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        throw std::runtime_error("failed to find suitable memory type!");
    }

    void App::handleWindowResize(GLFWwindow *window, int width, int height)
    {
        auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
        app->setFrameBufferResized();
    }

    void App::run()
    {
        init();

        while (!_window->shouldClose())
        {
            _window->pollEvents();
            update();
            render();
        }
        _device->waitIdle();
        cleanup();
    }

    void App::update()
    {
    }

    void App::render()
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
                _framebufferResized = true;
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
            _framebufferResized = true;
        }
#endif
        if (_framebufferResized)
        {
            _framebufferResized = false;
            recreateSwapChain();
            return;
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
                _framebufferResized = true;
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
            _framebufferResized = true;
        }
#endif
        _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        if (_framebufferResized)
        {
            _framebufferResized = false;
            recreateSwapChain();
        }
    }
    void App::recordCommandBuffer(const vk::raii::CommandBuffer &commandBuffer, uint32_t imageIndex)
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
        vk::DeviceSize offsets[] = {0};
        commandBuffer.bindVertexBuffers(0, vertexBuffers, offsets);

        commandBuffer.draw(static_cast<uint32_t>(_mesh.vertices.size()), 1, 0, 0);
        // commandBuffer.draw(3, 1, 0, 0);

        commandBuffer.endRenderPass();
        commandBuffer.end();
    }
}
