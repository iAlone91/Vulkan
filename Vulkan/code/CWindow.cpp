#include "CWindow.h"

// Std ///////////////////
#include <stdexcept>

Window::Window(uint32_t width, uint32_t height, std::string name)
: m_width(width), m_height(height), m_windowName(name)
{
	initWindow();
} //Window()

Window::~Window()
{
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
} //~Window()

void Window::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disables OpenGl context

	m_pWindow = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr); // #4 open in a speciffic monitor, #5 rellevant if using OpenGL
	glfwSetWindowUserPointer(m_pWindow, this);
	glfwSetFramebufferSizeCallback(m_pWindow, framebufferResizeCallback);
} //initWindow()

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	Window* pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	pWindow->m_framebufferResized = true;
	pWindow->m_width = width;
	pWindow->m_height = height;
} //framebufferResizeCallback()

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, m_pWindow, nullptr, surface) != VK_SUCCESS)
		throw std::runtime_error("faild to create window surface");

} //createWindowSurface()