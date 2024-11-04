#pragma once
#include <vector>
#include <vulkan/vulkan_raii.hpp>
namespace lava::rendering
{
    class AttributeDescriptionBuilder
    {
        public:
        AttributeDescriptionBuilder();
        ~AttributeDescriptionBuilder();

        AttributeDescriptionBuilder* withFloat();
        AttributeDescriptionBuilder* withVec2();
        AttributeDescriptionBuilder* withVec3();
        AttributeDescriptionBuilder* withVec4();
        AttributeDescriptionBuilder* withMat3();
        AttributeDescriptionBuilder* withMat4();

        private:
        std::vector<vk::VertexInputAttributeDescription> _attributes;
    };
}