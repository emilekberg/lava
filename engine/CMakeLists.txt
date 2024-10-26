cmake_minimum_required(VERSION 3.5)

project(lava_engine C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(SOURCES ${SOURCES})

# include_directories(lava_engine "src")
# file(GLOB_RECURSE sources "{CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp")
add_library(lava_engine "src/header.cpp" "src/app.cpp" "src/core/window.cpp")
include(FetchContent)

# ---
# fetch glm
# ---
message(STATUS "fetch glm")
FetchContent_Declare(glm
    GIT_REPOSITORY	https://github.com/g-truc/glm.git
)
FetchContent_MakeAvailable(glm)
FetchContent_GetProperties(glm)

# ---
#  fetch glfw
# ---
message(STATUS "fetch glfw")
FetchContent_Declare(glfw
    GIT_REPOSITORY	https://github.com/glfw/glfw.git
)
FetchContent_MakeAvailable(glfw)
FetchContent_GetProperties(glfw)

# --
# Include Vulkan
# --
message(STATUS "include vulkan")
find_package(Vulkan REQUIRED)
add_compile_definitions(GLFW_INCLUDE_VULKAN)


target_include_directories(lava_engine
    PUBLIC
    glm::glm
    glfw
    Vulkan::Vulkan
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(lava_engine
    PUBLIC
    glm::glm
    glfw
    Vulkan::Vulkan
)
target_compile_definitions(lava_engine PUBLIC GLFW_INCLUDE_VULKAN=${GLFW_INCLUDE_VULKAN})
target_compile_definitions(lava_engine PUBLIC VK_USE_PLATFORM_WIN32_KHR=${VK_USE_PLATFORM_WIN32_KHR})
target_compile_definitions(lava_engine PUBLIC GLFW_EXPOSE_NATIVE_WIN32=${GLFW_EXPOSE_NATIVE_WIN32})