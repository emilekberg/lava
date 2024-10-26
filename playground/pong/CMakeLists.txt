cmake_minimum_required(VERSION 3.5)

project(pong CXX)

add_executable(pong src/main.cpp)
# find_package(Vulkan REQUIRED)


target_link_libraries(pong
    PUBLIC
#    glm::glm
#    glfw
#    Vulkan::Vulkan
    lava_engine
)