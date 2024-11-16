#pragma once
#include <vector>
#include <string>
#include <tuple>
#include "lava/rendering/data/vertex.hpp"
#include "lava/rendering/data/mesh.hpp"
#include "lava/rendering/buffer.hpp"
#include "lava/rendering/data/texture.hpp"
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
    std::unique_ptr<rendering::Buffer> loadImageToStagingBuffer(std::string filepath, const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, int* width, int* height);
    std::tuple<std::unique_ptr<vk::raii::Image>, std::unique_ptr<vk::raii::DeviceMemory>> loadImageToTexture(std::string filepath, const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice);
    std::unique_ptr<rendering::data::Texture> loadImageToTexture2(std::string filepath, const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice);

}