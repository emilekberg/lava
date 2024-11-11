#pragma once
#include <vector>
#include <string>
#include "lava/rendering/data/vertex.hpp"
#include "lava/rendering/data/mesh.hpp"
namespace lava::resourceloader
{
    std::vector<char> readfile(const std::string& filepath);
    rendering::data::Mesh loadMesh(const std::string& filepath);
}