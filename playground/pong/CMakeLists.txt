cmake_minimum_required(VERSION 3.5)

project(pong)

add_executable(pong src/main.cpp)

target_link_libraries(pong
    lava_engine
)