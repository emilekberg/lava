#include "lava/rendering/device.hpp"

namespace lava::rendering
{
    Device::Device(vk::raii::Instance& instance) : 
        _device(VK_NULL_HANDLE), _physicalDevice(VK_NULL_HANDLE), _instance(instance)
    {
        
    }
    Device::~Device()
    {

    }
}