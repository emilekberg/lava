cmake_minimum_required(VERSION 3.5)
project(lava C CXX)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(engine)
# add_subdirectory(playground/pong)
enable_testing()
add_subdirectory(tests)