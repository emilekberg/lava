#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <memory>

namespace lava::rendering::constructors
{
    std::unique_ptr<vk::raii::SurfaceKHR> createSurface(const vk::raii::Instance& instance, HWND windowHandle)
    {
        vk::Win32SurfaceCreateInfoKHR createInfo{};
        createInfo.hwnd = windowHandle;
        createInfo.hinstance = GetModuleHandle(nullptr);
        return std::make_unique<vk::raii::SurfaceKHR>(instance, createInfo);
    }
}