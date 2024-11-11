#include <cstdio>
#include <lava/app.hpp>
#include <GLFW/glfw3.h>
#include "lava/resourceloader.hpp"
int main()
{
    



    try
    {
        glfwInit();
        auto app = lava::App();
        app.run();
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "Error: %s\n", e.what());
    }
    glfwTerminate();
    return 0;
}