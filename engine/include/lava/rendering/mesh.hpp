#pragma once
#include "lava/rendering/vertex.hpp"
#include <vector>
namespace lava::rendering
{
    struct Mesh
    {
        std::vector<Vertex> vertices;

        Mesh()
        {
            vertices = {
                // top triangle
                {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
                {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

                // bottom triangle
                {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                {{0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
                {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            };
        }
    };
}