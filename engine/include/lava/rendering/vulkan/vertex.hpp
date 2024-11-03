#pragma once
#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>
namespace lava::rendering::vulkan
{
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;

        static vk::VertexInputBindingDescription getBindingDescription()
        {
            vk::VertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = vk::VertexInputRate::eVertex;
            return bindingDescription;
        }

        static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
        {
            std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};
            attributeDescriptions[0].setBinding(0);
            attributeDescriptions[0].setLocation(0);
            attributeDescriptions[0].setFormat(vk::Format::eR32G32Sfloat);
            attributeDescriptions[0].setOffset(offsetof(Vertex, pos));

            attributeDescriptions[1].setBinding(0);
            attributeDescriptions[1].setLocation(1);
            attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
            attributeDescriptions[1].setOffset(offsetof(Vertex, color));
            return attributeDescriptions;
        }
    };

}
