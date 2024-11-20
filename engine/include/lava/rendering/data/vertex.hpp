#pragma once
#include <vulkan/vulkan.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtx/hash.hpp>
namespace lava::rendering::data
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;

        static vk::VertexInputBindingDescription getBindingDescription()
        {
            vk::VertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = vk::VertexInputRate::eVertex;
            return bindingDescription;
        }

        bool operator==(const Vertex& other) const
        {
            return (position == other.position && normal == other.normal && texCoord == other.texCoord);
        }

        static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
        {
            std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(3);
            attributeDescriptions[0].setBinding(0);
            attributeDescriptions[0].setLocation(0);
            attributeDescriptions[0].setFormat(vk::Format::eR32G32B32Sfloat);
            attributeDescriptions[0].setOffset(offsetof(Vertex, position));

            attributeDescriptions[1].setBinding(0);
            attributeDescriptions[1].setLocation(1);
            attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
            attributeDescriptions[1].setOffset(offsetof(Vertex, normal));

            attributeDescriptions[2].setBinding(0);
            attributeDescriptions[2].setLocation(2);
            attributeDescriptions[2].setFormat(vk::Format::eR32G32Sfloat);
            attributeDescriptions[2].setOffset(offsetof(Vertex, texCoord));
            return attributeDescriptions;
        }
    };
}
namespace std {
    template<> struct hash<lava::rendering::data::Vertex> {
        size_t operator()(lava::rendering::data::Vertex const& vertex) const {
            return ((std::hash<glm::vec3>()(vertex.position) ^
                   (std::hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
                   (std::hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}