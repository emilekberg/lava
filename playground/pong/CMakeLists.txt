cmake_minimum_required(VERSION 3.5)

project(pong CXX)

add_executable(pong src/main.cpp)
# find_package(Vulkan REQUIRED)

target_compile_definitions(pong PUBLIC GLFW_INCLUDE_VULKAN=${GLFW_INCLUDE_VULKAN})
target_compile_definitions(pong PUBLIC VK_USE_PLATFORM_WIN32_KHR=${VK_USE_PLATFORM_WIN32_KHR})
target_compile_definitions(pong PUBLIC GLFW_EXPOSE_NATIVE_WIN32=${GLFW_EXPOSE_NATIVE_WIN32})

target_link_libraries(pong
    PUBLIC
#    glm::glm
#    glfw
#    Vulkan::Vulkan
    lava_engine
)