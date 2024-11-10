#include "lava/rendering/builders/graphics-pipeline-builder.hpp"
#include "lava/resourceloader.hpp"
namespace lava::rendering::builders
{
    GraphicsPipelineBuilder::GraphicsPipelineBuilder(const vk::raii::Device &device)
        : _device(device)
    {
    }

    GraphicsPipelineBuilder::~GraphicsPipelineBuilder()
    {
    }

    GraphicsPipelineBuilder &GraphicsPipelineBuilder::withFragmentShader(const std::string& path)
    {
        auto fragShaderCode = lava::resourceloader::readfile(path);

        _fragmentShaderModule = createShaderModule(fragShaderCode);

        vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(_fragmentShaderModule.value())
            .setPName("main");

        _fragmentCreateInfo = fragShaderStageInfo;
        return *this;
    }
    
    GraphicsPipelineBuilder &GraphicsPipelineBuilder::withVertexShader(const std::string& path)
    {
        auto vertShaderCode = lava::resourceloader::readfile(path);

        _vertexShaderModule = createShaderModule(vertShaderCode);

        vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(_vertexShaderModule.value())
            .setPName("main");

        _vertexCreateInfo = vertShaderStageInfo;
        return *this;
    }

    GraphicsPipelineBuilder &GraphicsPipelineBuilder::withVertexInputInfo(const vk::VertexInputBindingDescription& bindingDescription, const std::vector<vk::VertexInputAttributeDescription>& attributeDescription)
    {
        _bindingDescription = std::move(bindingDescription);
        _attributeDescription = std::move(attributeDescription);

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.setVertexBindingDescriptionCount(1);
        vertexInputInfo.setPVertexBindingDescriptions(&_bindingDescription.value());
        vertexInputInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(_attributeDescription.value().size()));
        vertexInputInfo.setVertexAttributeDescriptions(_attributeDescription.value());
        _vertexInputInfo = vertexInputInfo;
        return *this;
    }

    GraphicsPipelineBuilder &GraphicsPipelineBuilder::withExtent(const vk::Extent2D& extent)
    {
        _extent = extent;
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::withRenderPass(std::shared_ptr<vk::raii::RenderPass> renderPass)
    {
        _renderPass = renderPass;
        return *this;
    }

    std::unique_ptr<GraphicsPipeline> GraphicsPipelineBuilder::build(const vk::raii::DescriptorSetLayout& layout)
    {
        vk::PipelineShaderStageCreateInfo shaderStages[] = {_vertexCreateInfo.value(), _fragmentCreateInfo.value()};
        std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));
        dynamicState.setPDynamicStates(dynamicStates.data());

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
        inputAssembly.setPrimitiveRestartEnable(vk::False);

        vk::Viewport viewport{};
        viewport.setX(0.0f);
        viewport.setY(0.0f);
        viewport.setWidth((float)_extent.value().width);
        viewport.setHeight((float)_extent.value().height);
        viewport.setMinDepth(0.0f);
        viewport.setMaxDepth(1.0f);

        vk::Rect2D scissor{};
        scissor.setOffset({0, 0});
        scissor.setExtent(_extent.value());

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.setViewportCount(1);
        viewportState.setPViewports(&viewport);
        viewportState.setScissorCount(1);
        viewportState.setPScissors(&scissor);

        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.setDepthClampEnable(vk::False);
        rasterizer.setRasterizerDiscardEnable(vk::False);
        rasterizer.setPolygonMode(vk::PolygonMode::eFill);
        rasterizer.setLineWidth(1.0f);

        rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
        rasterizer.setFrontFace(vk::FrontFace::eCounterClockwise);

        rasterizer.setDepthBiasEnable(vk::False);
        rasterizer.setDepthBiasConstantFactor(0.0f);
        rasterizer.setDepthBiasClamp(0.0f);
        rasterizer.setDepthBiasSlopeFactor(0.0f);

        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.setSampleShadingEnable(vk::False);
        multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
        multisampling.setMinSampleShading(1.0f);           // Optional
        multisampling.setPSampleMask(nullptr);             // Optional
        multisampling.setAlphaToCoverageEnable(vk::False); // Optional
        multisampling.setAlphaToOneEnable(vk::False);      // Optional

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
        colorBlendAttachment.setBlendEnable(vk::False);
        colorBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eOne);  // Optional
        colorBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eZero); // Optional
        colorBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);             // Optional
        colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eOne);  // Optional
        colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero); // Optional
        colorBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);             // Optional

        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.setLogicOpEnable(vk::False);
        colorBlending.setAttachmentCount(1);
        colorBlending.setPAttachments(&colorBlendAttachment);

        std::vector<vk::DescriptorSetLayout> layouts
        {
            layout
        };
        vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.setSetLayoutCount(1);
        pipelineLayoutCreateInfo.setPSetLayouts(&layouts[0]);

        vk::raii::PipelineLayout pipelineLayout = vk::raii::PipelineLayout(_device, pipelineLayoutCreateInfo);

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.setStageCount(2);
        pipelineInfo.setPStages(shaderStages);
        pipelineInfo.setPVertexInputState(&_vertexInputInfo.value());
        pipelineInfo.setPInputAssemblyState(&inputAssembly);
        pipelineInfo.setPViewportState(&viewportState);
        pipelineInfo.setPRasterizationState(&rasterizer);
        pipelineInfo.setPMultisampleState(&multisampling);
        pipelineInfo.setPDepthStencilState(nullptr);
        pipelineInfo.setPColorBlendState(&colorBlending);
        pipelineInfo.setPDynamicState(&dynamicState);

        pipelineInfo.setLayout(pipelineLayout);
        pipelineInfo.setRenderPass(*_renderPass.get());

        pipelineInfo.setSubpass(0);

        vk::raii::Pipeline pipeline(_device, nullptr, pipelineInfo);
        return std::make_unique<GraphicsPipeline>(std::move(pipeline), std::move(pipelineLayout));
    }

    vk::raii::ShaderModule GraphicsPipelineBuilder::createShaderModule(const std::vector<char> &code)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.setCodeSize(code.size());
        createInfo.setPCode(reinterpret_cast<const uint32_t *>(code.data()));
        return vk::raii::ShaderModule(_device, createInfo);
    }
}