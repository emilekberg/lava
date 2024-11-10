#pragma once
#include "lava/rendering/data/vertex.hpp"
#include <vector>
namespace lava::rendering::data
{
    struct Mesh
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::string pipeline = "default";

        Mesh()
        {
            
            vertices = {
                // bottom
                {{-1.0f, -1.0f, 0.0f},  {1.0f, 1.0f, 1.0f}},
                {{1.0, -1.0f, 0.0f},    {1.0f, 1.0f, 0.0f}},
                {{1.0f, 1.0f, 0.0f},    {1.0f, 0.0f, 1.0f}},
                {{-1.0f, 1.0f, 0.0f},   {0.0f, 1.0f, 1.0f}}
                
                
            };
            indices = {
                // bottom
                0, 1, 3,
                1, 2, 3
                
                
            };
        }
    };
}