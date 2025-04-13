project(pong CXX)
# find_package(Vulkan REQUIRED)

add_executable(pong src/main.cpp)
target_sources(pong
    PUBLIC
        FILE_SET CXX_MODULES FILES
        src/b.cppm
        src/a.cppm
)

target_compile_definitions(pong PUBLIC GLFW_INCLUDE_VULKAN=${GLFW_INCLUDE_VULKAN})
target_compile_definitions(pong PUBLIC VK_USE_PLATFORM_WIN32_KHR=${VK_USE_PLATFORM_WIN32_KHR})
target_compile_definitions(pong PUBLIC GLFW_EXPOSE_NATIVE_WIN32=${GLFW_EXPOSE_NATIVE_WIN32})

target_link_libraries(pong
    PUBLIC
    lava_engine
)