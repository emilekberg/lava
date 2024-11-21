#include "lava/rendering/data/vkMesh.hpp"

namespace lava::rendering::data
{
    VkMesh::VkMesh(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const Mesh& mesh)
        : VkMesh(device, physicalDevice, mesh.vertices, mesh.indices)
    {
        
    }
    VkMesh::VkMesh(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : _indexBuffer(device, physicalDevice, sizeof(indices[0]) * indices.size(), vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal), 
        _vertexBuffer(device, physicalDevice, sizeof(vertices[0]) * vertices.size(), vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal),
        _numIndices(indices.size())

    {
        
    }
}