project(lava_tests CXX)

include(FetchContent)
include(CTest)
include(GoogleTest)

enable_testing()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git 
    GIT_TAG        v1.15.2
)

FetchContent_MakeAvailable(googletest)

add_executable(lava_tests 
    test.cpp
    ecs.cpp
)
target_link_libraries(lava_tests
    gtest
    gtest_main
    lava_engine
)
gtest_discover_tests(lava_tests
    WORKING_DIRECTORY ${PROJECT_WORKING_DIRECTORY}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_WORKING_DIRECTORY}"
)