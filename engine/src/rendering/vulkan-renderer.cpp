#include "lava/rendering/vulkan-renderer.hpp"
#include "lava/rendering/builders/graphics-pipeline-builder.hpp"
#include "lava/rendering/constructors/buffer.hpp"
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
// #include "lava/rendering/constructors/texture-image.hpp"
#define UNHANDLED_PARAMETER(param) param;
#undef max
namespace lava::rendering
{
    VulkanRenderer::VulkanRenderer(const ScreenSize &screenSize, HWND windowHandle) : _validationLayers({"VK_LAYER_KHRONOS_validation"}),
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


        std::tie(_image, _imageMemory) = createTextureImage(*_device.get(), *_physicalDevice.get());
        _imageView = createTextureImageView();
        createTextureSampler();
 

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
        // cleanupSwapChain();
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
        vk::raii::Buffer stagingBuffer = VK_NULL_HANDLE;
        vk::raii::DeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

        std::tie(stagingBuffer, stagingBufferMemory) = constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        void *data = stagingBufferMemory.mapMemory(0, bufferSize);
        memcpy(data, _mesh.vertices.data(), (size_t)bufferSize);
        stagingBufferMemory.unmapMemory();

        vk::raii::Buffer vertexBuffer = VK_NULL_HANDLE;
        vk::raii::DeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
        std::tie(vertexBuffer, vertexBufferMemory) = constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
        copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
        
        _vertexBuffer = std::make_unique<vk::raii::Buffer>(std::move(vertexBuffer));
        _vertexBufferMemory = std::make_unique<vk::raii::DeviceMemory>(std::move(vertexBufferMemory));
    }

    void VulkanRenderer::createIndexBuffers()
    {
        vk::DeviceSize bufferSize = sizeof(_mesh.indices[0]) * _mesh.indices.size();
        vk::raii::Buffer stagingBuffer = VK_NULL_HANDLE;
        vk::raii::DeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

        std::tie(stagingBuffer, stagingBufferMemory) = constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        void *data = stagingBufferMemory.mapMemory(0, bufferSize);
        memcpy(data, _mesh.indices.data(), (size_t)bufferSize);
        stagingBufferMemory.unmapMemory();

        vk::raii::Buffer indexBuffer = VK_NULL_HANDLE;
        vk::raii::DeviceMemory indexBufferMemory = VK_NULL_HANDLE;
        std::tie(indexBuffer, indexBufferMemory) = constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
        copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        _indexBuffer = std::make_unique<vk::raii::Buffer>(std::move(indexBuffer));
        _indexBufferMemory = std::make_unique<vk::raii::DeviceMemory>(std::move(indexBufferMemory));

    }

    void VulkanRenderer::createUniformBuffers()
    {
        vk::DeviceSize bufferSize = sizeof(data::UniformBufferObject);
        _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        _uniformBufferMemories.resize(MAX_FRAMES_IN_FLIGHT);
        _uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vk::raii::Buffer uniformBuffer = VK_NULL_HANDLE;
            vk::raii::DeviceMemory uniformBufferMemory = VK_NULL_HANDLE;
            std::tie(uniformBuffer, uniformBufferMemory) = constructors::createBuffer(*_device.get(), *_physicalDevice.get(), bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
            _uniformBuffers[i] = std::make_unique<vk::raii::Buffer>(std::move(uniformBuffer));
            _uniformBufferMemories[i] = std::make_unique<vk::raii::DeviceMemory>(std::move(uniformBufferMemory));

            _uniformBuffersMapped[i] = _uniformBufferMemories[i]->mapMemory(0, bufferSize);
        }
    }

    void VulkanRenderer::createDescriptorPool()
    {
        std::array<vk::DescriptorPoolSize, 2> poolSizes{};
        poolSizes[0]
            .setType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));
        poolSizes[1]
            .setType(vk::DescriptorType::eCombinedImageSampler)
            .setDescriptorCount(static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo
            .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setPoolSizeCount(static_cast<uint32_t>(poolSizes.size()))
            .setPPoolSizes(poolSizes.data())
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

            vk::DescriptorImageInfo imageInfo{};
            imageInfo
                .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                .setImageView(*_imageView.get())
                .setSampler(*_sampler.get());

            std::array<vk::WriteDescriptorSet, 2> descriptorWrites{};
        
            descriptorWrites[0]
                .setDstSet(_descriptorSets->at(i))
                .setDstBinding(0)
                .setDstArrayElement(0)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDescriptorCount(1)
                .setPBufferInfo(&bufferInfo)
                .setPImageInfo(nullptr)
                .setPTexelBufferView(nullptr);

            descriptorWrites[1]
                .setDstSet(_descriptorSets->at(i))
                .setDstBinding(1)
                .setDstArrayElement(0)
                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setDescriptorCount(1)
                .setPImageInfo(&imageInfo);

            _device->updateDescriptorSets(descriptorWrites, 0);
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

        vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding
            .setBinding(1)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            .setPImmutableSamplers(nullptr)
            .setStageFlags(vk::ShaderStageFlagBits::eFragment);

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo
            .setBindingCount(static_cast<uint32_t>(bindings.size()))
            .setPBindings(bindings.data());

        _descriptorSetLayout = std::make_unique<vk::raii::DescriptorSetLayout>(*_device.get(), layoutInfo);
    }

    void VulkanRenderer::requireResize()
    {
        _requiresResize = true;
    }

    void VulkanRenderer::beginSingleTimeCommands(std::function<void(const vk::raii::CommandBuffer &)> callback)
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setCommandPool(*_commandPool.get());
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        allocInfo.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);

        vk::raii::CommandBuffers commandBuffers(*_device.get(), allocInfo);

        vk::raii::CommandBuffer commandBuffer = std::move(commandBuffers[0]);

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commandBuffer.begin(beginInfo);
        {
            callback(commandBuffer);
        }

        commandBuffer.end();
        vk::CommandBuffer tmpCommandBuffers[] = {commandBuffer};
        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(tmpCommandBuffers);

        _graphicsQueue->submit({submitInfo}, VK_NULL_HANDLE);
        _graphicsQueue->waitIdle();
        // return std::move(commandBuffer);
    }

    void VulkanRenderer::endSingleTimeCommands(const vk::raii::CommandBuffer &commandBuffer)
    {
        vk::CommandBuffer tmpCommandBuffers[] = {commandBuffer};
        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(tmpCommandBuffers);

        _graphicsQueue->submit({submitInfo}, VK_NULL_HANDLE);
        _graphicsQueue->waitIdle();
    }

    /*void VulkanRenderer::copyBuffer2(const vk::raii::Buffer &sourceBuffer, const vk::raii::Buffer &destinationBuffer, vk::DeviceSize size)
    {
        const vk::raii::CommandBuffer &commandBuffer = beginSingleTimeCommands();

        vk::BufferCopy copyRegion{};
        copyRegion.size = size;
        commandBuffer.copyBuffer(sourceBuffer, destinationBuffer, {copyRegion});

        endSingleTimeCommands(std::move(commandBuffer));
    }*/

    void VulkanRenderer::transitionImageLayout(const vk::Image &image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
    {
        beginSingleTimeCommands([&](const vk::raii::CommandBuffer &commandBuffer)
                                {
            vk::ImageMemoryBarrier barrier{};
            barrier
                .setOldLayout(oldLayout)
                .setNewLayout(newLayout)
                .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setImage(image)
                .subresourceRange
                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseMipLevel(0)
                .setLevelCount(1)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

            vk::PipelineStageFlags sourceStage;
            vk::PipelineStageFlags destinationStage;

            if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
            {
                barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
                barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

                sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
                destinationStage = vk::PipelineStageFlagBits::eTransfer;
            }
            else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
            {
                barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
                barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

                sourceStage = vk::PipelineStageFlagBits::eTransfer;
                destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
            }
            else
            {
                throw new std::invalid_argument("unsupported layout transition!");
            }

            barrier;
            //.setSrcAccessMask(0)
            //.setDstAccessMask(0);

            commandBuffer.pipelineBarrier(
                sourceStage, destinationStage,
                vk::Flags<vk::DependencyFlagBits>(),
                0,
                0,
                barrier); });
    }

    void VulkanRenderer::copyBufferToImage(const vk::raii::Buffer &buffer, const vk::raii::Image &image, uint32_t width, uint32_t height)
    {
        beginSingleTimeCommands([&](const vk::raii::CommandBuffer &commandBuffer)
                                {
            vk::BufferImageCopy region{};
            region
                .setBufferOffset(0)
                .setBufferRowLength(0)
                .setBufferImageHeight(0);

            region.imageSubresource
                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setMipLevel(0)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

            region
                .setImageOffset({0, 0, 0})
                .setImageExtent({width, height, 1}); 
                
                commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, {region});
                
                });



    }

    std::tuple<std::unique_ptr<vk::raii::Image>, std::unique_ptr<vk::raii::DeviceMemory>> VulkanRenderer::createImage(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties)
    {
        vk::ImageCreateInfo imageInfo{};
        imageInfo
            .setImageType(vk::ImageType::e2D)
            .setExtent({width, height, 1})
            .setMipLevels(1)
            .setArrayLayers(1)
            .setFormat(format)
            .setTiling(tiling)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setUsage(usage)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSamples(vk::SampleCountFlagBits::e1);

        std::unique_ptr<vk::raii::Image> textureImage = std::make_unique<vk::raii::Image>(device, imageInfo);

        auto memoryRequirements = textureImage->getMemoryRequirements();

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo
            .setAllocationSize(memoryRequirements.size)
            .setMemoryTypeIndex(constructors::findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties));

        std::unique_ptr<vk::raii::DeviceMemory> textureMemory = std::make_unique<vk::raii::DeviceMemory>(std::move(device.allocateMemory(allocInfo)));
        textureImage->bindMemory(*textureMemory.get(), 0);

        return std::make_tuple(std::move(textureImage), std::move(textureMemory));
    }
    std::tuple<std::unique_ptr<vk::raii::Image>, std::unique_ptr<vk::raii::DeviceMemory>> VulkanRenderer::createTextureImage(const vk::raii::Device &device, const vk::raii::PhysicalDevice &physicalDevice)
    {
        // std::tie(image, memory) = resourceloader::loadImageToTexture("./build/mesh/viking_room.png", device, physicalDevice);
        std::unique_ptr<rendering::Buffer> buffer = resourceloader::loadImageToStagingBuffer("./build/mesh/viking_room.png", device, physicalDevice);
        int width = 1024;
        int height = 1024;

        std::unique_ptr<vk::raii::Image> image;
        std::unique_ptr<vk::raii::DeviceMemory> imageMemory;
        std::tie(image, imageMemory) = createImage(device, physicalDevice, width, height, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal);

        transitionImageLayout(*image.get(), vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        copyBufferToImage(buffer->getVkBuffer(), *image.get(), static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        transitionImageLayout(*image.get(), vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        return std::make_tuple(std::move(image), std::move(imageMemory));
    }

    std::unique_ptr<vk::raii::ImageView> VulkanRenderer::createTextureImageView()
    {
        return std::make_unique<vk::raii::ImageView>(constructors::createImageView(*_device.get(), *_image.get(), vk::Format::eR8G8B8A8Srgb));
    }

    void VulkanRenderer::createTextureSampler()
    {
        auto properties = _physicalDevice->getProperties();

        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo
            .setMagFilter(vk::Filter::eLinear)
            .setMinFilter(vk::Filter::eLinear)
            .setAddressModeU(vk::SamplerAddressMode::eRepeat)
            .setAddressModeV(vk::SamplerAddressMode::eRepeat)
            .setAddressModeW(vk::SamplerAddressMode::eRepeat)
            .setAnisotropyEnable(vk::True)
            .setMaxAnisotropy(properties.limits.maxSamplerAnisotropy)
            .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
            .setUnnormalizedCoordinates(vk::False)
            .setCompareEnable(vk::False)
            .setCompareOp(vk::CompareOp::eAlways)
            .setMipmapMode(vk::SamplerMipmapMode::eLinear)
            .setMipLodBias(0.0f)
            .setMinLod(0.0f)
            .setMaxLod(0.0f);

        _sampler = std::make_unique<vk::raii::Sampler>(*_device.get(), samplerInfo);
            
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
        const vk::raii::CommandBuffer &commandBuffer = _commandBuffers->at(_currentFrame);
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