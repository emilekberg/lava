#pragma once
#include <glm/glm.hpp>
namespace lava::rendering::data
{
    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };
}