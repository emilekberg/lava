#pragma once
#include <vector>
#include <string>
namespace lava::resourceloader
{
    std::vector<char> readfile(const std::string& filepath);
}