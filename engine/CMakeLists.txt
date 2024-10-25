cmake_minimum_required(VERSION 3.5)

project(lava_engine C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(SOURCES ${SOURCES})

add_library(lava_engine "src/header.cpp")

target_include_directories(lava_engine
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
