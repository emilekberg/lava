#pragma once
#include "lava/rendering/vertex.hpp"
#include <vector>
namespace lava::rendering
{
    struct Mesh
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        Mesh()
        {
            vertices = {
                // top triangle
                // {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
                // {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                // {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

                // bottom triangle
                // {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                // {{0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
                // {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            
                    {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
                    {{0.5, -0.5f}, {1.0f, 1.0f, 0.0f}},
                    {{0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},
                    {{-0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}}
            };
            indices = {
                0, 1, 3, 
                1, 2, 3
            };

        }
    };
}