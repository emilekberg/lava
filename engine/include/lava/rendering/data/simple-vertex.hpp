#pragma once
#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>
namespace lava::rendering::data
{
    struct SimpleVertex
    {
        glm::vec3 pos;
        glm::vec3 color;

        static vk::VertexInputBindingDescription getBindingDescription()
        {
            vk::VertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(SimpleVertex);
            bindingDescription.inputRate = vk::VertexInputRate::eVertex;
            return bindingDescription;
        }

        static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
        {
            std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(2);
            attributeDescriptions[0].setBinding(0);
            attributeDescriptions[0].setLocation(0);
            attributeDescriptions[0].setFormat(vk::Format::eR32G32B32Sfloat);
            attributeDescriptions[0].setOffset(offsetof(SimpleVertex, pos));

            attributeDescriptions[1].setBinding(0);
            attributeDescriptions[1].setLocation(1);
            attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
            attributeDescriptions[1].setOffset(offsetof(SimpleVertex, color));
            return attributeDescriptions;
        }
    };

}
