#include <lava/core/window.hpp>
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
     
     std::tuple<int, int> Window::getSize() const
     {
          int width, height;
          glfwGetFramebufferSize(_glfwWindow, &width, &height);
          return std::make_tuple(width, height);
     }
     rendering::ScreenSize Window::getScreenSize() const
     {
          rendering::ScreenSize screenSize{};
          glfwGetFramebufferSize(_glfwWindow, &screenSize.width, &screenSize.height);
          return screenSize;
     }

     HWND Window::getWindowHandle() const
     {
#ifdef GLFW_EXPOSE_NATIVE_WIN32
          return glfwGetWin32Window(_glfwWindow);
#endif
     }
     GLFWwindow* Window::getGLFWwindow()
     {
          return _glfwWindow;
     }
     template <typename T>
     void Window::setOwner(T* owner)
     {
          glfwSetWindowUserPointer(_glfwWindow, owner);
     }
     void Window::setResizeHandler(void(callback)(GLFWwindow* window, int width, int height))
     {
          glfwSetFramebufferSizeCallback(_glfwWindow, callback);
     }
}