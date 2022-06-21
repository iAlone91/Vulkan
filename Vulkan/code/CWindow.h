#pragma once

// GLFW /////////////////////////////////
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Std //////////////////////////////////
#include <string>
class Window
{
public:

	Window(uint32_t width, uint32_t height, std::string name);
	~Window();


	// Delete the copy constructor
	Window(const Window&) = delete;
	Window &operator = (const Window&) = delete;

	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	// Getters //////////////////////////////
	bool shouldClose() { return glfwWindowShouldClose(m_pWindow); }
	GLFWwindow* getWindow() const { return m_pWindow; }
	VkExtent2D getExtent() { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) }; }
	bool wasWindowResized() { return m_framebufferResized; }
	void resetWindowResizeFlag() { m_framebufferResized = false; }
	GLFWwindow* getGLFWwindow() const { return m_pWindow; }

private:

	void initWindow();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	int		m_width;
	int		m_height;
	bool	m_framebufferResized		= false;

	std::string		m_windowName{};
	GLFWwindow*		m_pWindow{};
};