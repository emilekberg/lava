#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <vector>
#include "lava/rendering/graphics-pipeline.hpp"
#include <optional>
#include <memory>
namespace lava::rendering::builders
{
    class GraphicsPipelineBuilder
    {
    public:
        GraphicsPipelineBuilder(const vk::raii::Device& device);
        ~GraphicsPipelineBuilder();
        GraphicsPipelineBuilder& withFragmentShader();
        GraphicsPipelineBuilder& withVertexShader();
        GraphicsPipelineBuilder& withVertexInputInfo();
        GraphicsPipelineBuilder& withExtent(const vk::Extent2D& extent);
        GraphicsPipelineBuilder& withRenderPass(std::shared_ptr<vk::raii::RenderPass> renderPass);

        std::unique_ptr<GraphicsPipeline> build();

    private:
        vk::raii::ShaderModule createShaderModule(const std::vector<char> &code);


        std::optional<vk::PipelineShaderStageCreateInfo> _vertexCreateInfo;
        std::optional<vk::raii::ShaderModule> _vertexShaderModule;
        std::optional<vk::PipelineShaderStageCreateInfo> _fragmentCreateInfo;
        std::optional<vk::raii::ShaderModule> _fragmentShaderModule;

        std::optional<vk::VertexInputBindingDescription> _bindingDescription;
        std::optional<std::array<vk::VertexInputAttributeDescription, 2>> _attributeDescription;
        std::optional<vk::PipelineVertexInputStateCreateInfo> _vertexInputInfo;
        
        
        std::optional<vk::Extent2D> _extent;
        std::shared_ptr<vk::raii::RenderPass> _renderPass;

        const vk::raii::Device& _device; 
    };
}