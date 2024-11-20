cmake_minimum_required(VERSION 3.5)

project(lava_engine C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(SOURCES ${SOURCES})

# include_directories(lava_engine "src")
file(GLOB_RECURSE LAVA_SOURCES CONFIGURE_DEPENDS "**/*.cpp")
add_library(lava_engine ${LAVA_SOURCES})
# add_library(lava_engine "src/header.cpp" "src/app.cpp" "src/core/window.cpp")
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

# --
# Include imgui
# --
# message(STATUS "include imgui")
# FetchContent_Declare(imgui
#     GIT_REPOSITORY https://github.com/ocornut/imgui.git
# )
# FetchContent_MakeAvailable(imgui)
# FetchContent_GetProperties(imgui)


target_include_directories(lava_engine
    PUBLIC
    glm::glm
    glfw
    Vulkan::Vulkan
    imgui
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(lava_engine
    PUBLIC
    glm::glm
    glfw
    Vulkan::Vulkan
    # imgui
)
target_compile_definitions(lava_engine PUBLIC GLFW_INCLUDE_VULKAN=1)
target_compile_definitions(lava_engine PUBLIC VK_USE_PLATFORM_WIN32_KHR=1)
target_compile_definitions(lava_engine PUBLIC GLFW_EXPOSE_NATIVE_WIN32=1)
target_compile_definitions(lava_engine PUBLIC STB_IMAGE_IMPLEMENTATION=1)
target_compile_definitions(lava_engine PUBLIC GLM_FORCE_RADIANS=1)
target_compile_definitions(lava_engine PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE=1)
target_compile_definitions(lava_engine PUBLIC GLM_ENABLE_EXPERIMENTAL=1)
target_compile_definitions(lava_engine PUBLIC TINYOBJLOADER_IMPLEMENTATION=1)

# file(COPY ./resources/basic_vert.spv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# file(COPY ./resources/basic_frag.spv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
