#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_raii.hpp>
namespace lava::rendering
{
    class Shader
    {
    public:
        Shader(const vk::raii::Device& device, const std::string& path);
        ~Shader();
    private:
        static vk::raii::ShaderModule createShaderModule(const vk::raii::Device& device, const std::vector<char>& code);


        vk::raii::ShaderModule _module;
    };
}