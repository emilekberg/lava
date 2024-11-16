#pragma once
#include <vector>
#include <string>
#include <tuple>
#include "lava/rendering/data/vertex.hpp"
#include "lava/rendering/data/mesh.hpp"
#include "lava/rendering/buffer.hpp"
namespace lava::resourceloader
{
    struct ImageBufferResult
    {
        int width;
        int height;
        const rendering::Buffer& buffer;
    };

    std::vector<char> readfile(const std::string& filepath);
    rendering::data::Mesh loadMesh(const std::string& filepath);
    std::unique_ptr<rendering::Buffer> loadImageToStagingBuffer(std::string filepath, const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice);
    std::tuple<std::unique_ptr<vk::raii::Image>, std::unique_ptr<vk::raii::DeviceMemory>> loadImageToTexture(std::string filepath, const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice);
}