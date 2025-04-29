//
// Created by Sayama on 29/04/2025.
//

#include <GLFW/glfw3.h>

#include "GLFWWindow.hpp"

namespace Imagine::Core {
	void * GLFWWindow::GetNativeWindow() {
		return m_Window;
	}

	bool GLFWWindow::WindowHasResized() {
		return frameBufferResized;
	}

	void GLFWWindow::framebufferResizeCallback(void* window, int width, int height) {
		auto app = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(static_cast<GLFWwindow *>(window)));
		app->frameBufferResized = true;
	}

	GLFWWindow::GLFWWindow(const std::string& windowName, const WindowParameters parameters) : Window{} {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, parameters.Resizable ? GLFW_TRUE : GLFW_FALSE);
		m_Window = glfwCreateWindow(parameters.Width, parameters.Height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(static_cast<GLFWwindow *>(m_Window), this);
		glfwSetFramebufferSizeCallback(static_cast<GLFWwindow *>(m_Window), reinterpret_cast<GLFWframebuffersizefun>(framebufferResizeCallback));
	}

	GLFWWindow::~GLFWWindow() {
		glfwDestroyWindow(static_cast<GLFWwindow *>(m_Window));
		glfwTerminate();
	}

	void GLFWWindow::Update() {
		glfwPollEvents();
	}

	uint32_t GLFWWindow::GetWindowWidth() {
		int width, height;
		glfwGetWindowSize(static_cast<GLFWwindow *>(m_Window), &width, &height);
		return width;
	}

	uint32_t GLFWWindow::GetWindowHeight() {
		int width, height;
		glfwGetWindowSize(static_cast<GLFWwindow *>(m_Window), &width, &height);
		return height;
	}

	uint32_t GLFWWindow::GetFramebufferWidth() {
		int width, height;
		glfwGetFramebufferSize(static_cast<GLFWwindow *>(m_Window), &width, &height);
		return width;
	}

	uint32_t GLFWWindow::GetFramebufferHeight() {
		int width, height;
		glfwGetFramebufferSize(static_cast<GLFWwindow *>(m_Window), &width, &height);
		return height;
	}
}
