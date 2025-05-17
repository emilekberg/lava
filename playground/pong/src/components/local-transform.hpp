#pragma once
#include <glm/vec3.hpp>

namespace pong::components
{
    struct LocalTransform
    {
        bool isDirty = true;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };
}