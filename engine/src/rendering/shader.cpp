#include "lava/rendering/shader.hpp"
#include "lava/resourceloader.hpp"
namespace lava::rendering
{
    Shader::Shader(const vk::raii::Device& device, const std::string& path)
        : _module{VK_NULL_HANDLE}
    {
        auto code = resourceloader::readfile(path);
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.setCodeSize(code.size());
        createInfo.setPCode(reinterpret_cast<const uint32_t *>(code.data()));
        _module = vk::raii::ShaderModule(device, createInfo);
    }

    Shader::~Shader()
    {

    }
}