#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <string>
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
        GraphicsPipelineBuilder& withFragmentShader(const std::string& path);
        GraphicsPipelineBuilder& withVertexShader(const std::string& path);
        GraphicsPipelineBuilder& withVertexInputInfo(const vk::VertexInputBindingDescription& bindingDescription, const std::vector<vk::VertexInputAttributeDescription>& attributeDescription);
        GraphicsPipelineBuilder& withExtent(const vk::Extent2D& extent);
        GraphicsPipelineBuilder& withRenderPass(const vk::raii::RenderPass& renderPass);
        std::unique_ptr<GraphicsPipeline> build(const vk::raii::DescriptorSetLayout& layout, const vk::raii::RenderPass& renderpass);

    private:
        vk::raii::ShaderModule createShaderModule(const std::vector<char> &code);

        std::optional<vk::PipelineShaderStageCreateInfo> _vertexCreateInfo;
        std::optional<vk::raii::ShaderModule> _vertexShaderModule;
        std::optional<vk::PipelineShaderStageCreateInfo> _fragmentCreateInfo;
        std::optional<vk::raii::ShaderModule> _fragmentShaderModule;

        std::optional<vk::VertexInputBindingDescription> _bindingDescription;
        std::optional<std::vector<vk::VertexInputAttributeDescription>> _attributeDescription;
        std::optional<vk::PipelineVertexInputStateCreateInfo> _vertexInputInfo;
        
        std::optional<vk::Extent2D> _extent;
        // vk::raii::RenderPass& _renderPass;

        const vk::raii::Device& _device; 
    };
}