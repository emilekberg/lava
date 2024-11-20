#include <cstdio>
#include <lava/app.hpp>
#include <GLFW/glfw3.h>
#include "lava/resourceloader.hpp"
#include <iostream>
 
int main(int argc, char* argv[])
{
    fprintf(stdout, argv[0]);
    try
    {
        glfwInit();
        auto app = lava::App();
        app.run();
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "Error: %s\n", e.what());
        std::cin.get();
    }
    glfwTerminate();
    return 0;
}