#include "lava/rendering/attribute-description-builder.hpp"

namespace lava::rendering
{
    AttributeDescriptionBuilder::AttributeDescriptionBuilder()
    {

    }
    
    AttributeDescriptionBuilder::~AttributeDescriptionBuilder()
    {

    }

    
    AttributeDescriptionBuilder* AttributeDescriptionBuilder::withFloat()
    {
        vk::VertexInputAttributeDescription desc{};
        desc.setBinding(0);
        desc.setLocation(_attributes.size());
        desc.setFormat(vk::Format::eR32Sfloat);
        desc.setOffset(0); // figure out
        return this;
    }
    AttributeDescriptionBuilder* AttributeDescriptionBuilder::withVec2()
    {
        return this;
    }
    AttributeDescriptionBuilder* AttributeDescriptionBuilder::withVec3()
    {
        return this;
    }
    AttributeDescriptionBuilder* AttributeDescriptionBuilder::withVec4()
    {
        return this;
    }
    AttributeDescriptionBuilder* AttributeDescriptionBuilder::withMat3()
    {
        return this;
    }
    AttributeDescriptionBuilder* AttributeDescriptionBuilder::withMat4()
    {
        return this;
    }
}