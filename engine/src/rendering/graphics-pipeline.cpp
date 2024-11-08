#include "lava/rendering/graphics-pipeline.hpp"

namespace lava::rendering
{
    GraphicsPipeline::GraphicsPipeline(vk::raii::Pipeline pipeline, vk::raii::PipelineLayout pipelineLayout) 
        : _pipeline(std::move(pipeline)), _pipelineLayout(std::move(pipelineLayout))
    {

    }

    GraphicsPipeline::~GraphicsPipeline()
    {

    }

    const vk::raii::Pipeline& GraphicsPipeline::getVkPipeline()
    {
        return _pipeline;
    }

    const vk::raii::PipelineLayout& GraphicsPipeline::getVkPipelineLayout()
    {
        return _pipelineLayout;
    }
}
