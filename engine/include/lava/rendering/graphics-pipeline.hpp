#pragma once
#include <vulkan/vulkan_raii.hpp>
namespace lava::rendering
{
    class GraphicsPipeline
    {
    public:
        GraphicsPipeline(vk::raii::Pipeline pipeline, vk::raii::PipelineLayout pipelineLayout);
        ~GraphicsPipeline();

        const vk::raii::Pipeline& getVkPipeline();
        const vk::raii::PipelineLayout& getVkPipelineLayout(); 
    private:
        vk::raii::Pipeline _pipeline;
        vk::raii::PipelineLayout _pipelineLayout;
    };
}