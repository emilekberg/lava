#pragma once
#include "lava/rendering/data/mesh.hpp"
#include "lava/rendering/buffer.hpp"
#include <vulkan/vulkan_raii.hpp>
namespace lava::rendering::data
{
    class VkMesh
    {
        public:
            VkMesh(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const Mesh& mesh);
            VkMesh(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

            const Buffer& getVertexBuffer() const
            {
                return _vertexBuffer;
            }
            const Buffer& getIndexBuffer() const
            {
                return _indexBuffer;
            }
            const uint32_t getNumIndices() const
            {
                return _numIndices;
            }
        private:
            Buffer _vertexBuffer;
            Buffer _indexBuffer;
            uint32_t _numIndices;
    };
}