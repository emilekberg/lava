#pragma once
#include "lava/rendering/data/vertex.hpp"
#include <vector>
namespace lava::rendering::data
{
    struct Mesh
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        Mesh()
        {
        }
        Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) : vertices(vertices), indices(indices)
        {
        }
    };
}