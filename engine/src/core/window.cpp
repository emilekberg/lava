#include <lava/core/window.h>
namespace lava::core
{
     Window::Window()
     {
          glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
          glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
          glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
          _glfwWindow = glfwCreateWindow(800, 600, "Lava", NULL, NULL);
          if (!_glfwWindow)
          {
               glfwTerminate();
               exit(EXIT_FAILURE);
               // Window or OpenGL context creation failed.
          }
     }
     Window::~Window()
     {
          glfwDestroyWindow(_glfwWindow);
     }

     void Window::close()
     {
          glfwSetWindowShouldClose(_glfwWindow, GLFW_TRUE);
     }
     bool Window::shouldClose()
     {
          return glfwWindowShouldClose(_glfwWindow);
     }
     void Window::pollEvents()
     {
          glfwPollEvents();
     }
     void Window::activate()
     {
          glfwMakeContextCurrent(_glfwWindow);
     }
     HWND Window::getWindowHandle()
     {
#ifdef GLFW_EXPOSE_NATIVE_WIN32
          return glfwGetWin32Window(_glfwWindow);
#endif
     }
     GLFWwindow* Window::getGLFWwindow()
     {
          return _glfwWindow;
     }
}