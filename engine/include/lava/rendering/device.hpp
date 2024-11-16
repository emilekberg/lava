#pragma once

#include <vulkan/vulkan_raii.hpp>
namespace lava::rendering
{
    class Device
    {
        public:
        Device(vk::raii::Instance& vkInstance);
        ~Device();

        private:
        vk::raii::Instance& _instance;
        vk::raii::Device _device;
        vk::raii::PhysicalDevice _physicalDevice;
    };
}