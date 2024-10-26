#include <lava/core/window.h>
namespace lava::core
{
   Window::Window()
   {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		_windowHandle = glfwCreateWindow(640, 480, "Lava", NULL, NULL);
		if (!_windowHandle)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
			// Window or OpenGL context creation failed.
		}
   }
   Window::~Window()
   {
        glfwDestroyWindow(_windowHandle);
   }

   void Window::close()
   {
        glfwSetWindowShouldClose(_windowHandle, GLFW_TRUE);
   }
   bool Window::shouldClose()
   {
        return glfwWindowShouldClose(_windowHandle);
   }
   void Window::pollEvents()
   {
        glfwPollEvents();
   }
   void Window::activate()
   {
        glfwMakeContextCurrent(_windowHandle);
   }
}