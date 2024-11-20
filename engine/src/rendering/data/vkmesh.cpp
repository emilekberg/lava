#include "lava/rendering/data/vkMesh.hpp"

namespace lava::rendering::data
{
    VkMesh::VkMesh(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const Mesh& mesh)
        : VkMesh(device, physicalDevice, mesh.vertices, mesh.indices)
    {
        
    }
    VkMesh::VkMesh(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : _indexBuffer(device, physicalDevice, sizeof(indices[0]) * indices.size(), vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal), 
        _vertexBuffer(device, physicalDevice, sizeof(vertices[0]) * vertices.size(), vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal) 

    {
        vk::DeviceSize indexSize = sizeof(indices[0]) * indices.size();
        Buffer stagingBuffer(device, physicalDevice, indexSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.mapMemory(0, indexSize, [&](void *memory)
                                { memcpy(memory, indices.data(), (size_t)indexSize); });
        // copyBuffer(stagingBuffer.getVkBuffer(), _indexBuffer.getVkBuffer(), indexSize);
        
        vk::DeviceSize vertexSize = sizeof(vertices[0]) * vertices.size();
        Buffer stagingBuffer2(device, physicalDevice, vertexSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer2.mapMemory(0, vertexSize, [&](void *memory)
                                { memcpy(memory, vertices.data(), (size_t)vertexSize); });
         // copyBuffer(stagingBuffer.getVkBuffer(), _indexBuffer.getVkBuffer(), indexSize);

    }
}