cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(engine)
add_subdirectory(playground/pong)
enable_testing()
add_subdirectory(tests)