//
// Created by Sayama on 29/04/2025.
//


#include "GLFWWindow.hpp"
#include <GLFW/glfw3.h>

#include "Imagine/Rendering/Renderer.hpp"
#include "Imagine/Scene/SceneManager.hpp"

using namespace Imagine::Core;

namespace Imagine::GLFW {
	void *GLFWWindow::GetNativeWindow() {
		return m_Window;
	}


	bool GLFWWindow::ShouldClose() {
		return glfwWindowShouldClose(m_Window);
	}

	void *GLFWWindow::GetWindowPtr() {
		return m_Window;
	}
	Core::MouseInput &GLFWWindow::GetMouse() {
		//TODO: Implement input system properly
	}

	void GLFWWindow::framebufferResizeCallback(void *window, int width, int height) {
		auto app = reinterpret_cast<GLFWWindow *>(glfwGetWindowUserPointer(static_cast<GLFWwindow *>(window)));
		app->frameBufferResized = true;
	}

	bool GLFWWindow::IsMinimized() {
		const int iconified = glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED);
		return iconified == GLFW_TRUE;
	}

	static inline void GLFWOnDrop(GLFWwindow* window, int path_count, const char* paths[]) {
		auto renderer = Renderer::Get();
		if (!renderer) return;
		auto scenePtr = SceneManager::GetMainScene().get();
		for (int i = 0; i < path_count; ++i) {
			renderer->LoadExternalModelInScene(paths[i], scenePtr);
		}
	}

	GLFWWindow::GLFWWindow(const std::string &windowName, const WindowParameters parameters) :
		Window{} {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // TODO: See if I want the No API by default.
		glfwWindowHint(GLFW_RESIZABLE, parameters.Resizable ? GLFW_TRUE : GLFW_FALSE);
		m_Window = glfwCreateWindow(parameters.Width, parameters.Height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetFramebufferSizeCallback(m_Window, reinterpret_cast<GLFWframebuffersizefun>(framebufferResizeCallback));

		glfwSetDropCallback(m_Window, &GLFWOnDrop);
	}

	GLFWWindow::~GLFWWindow() {
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void GLFWWindow::Update() {
		glfwPollEvents();

	}

	Vec2 GLFWWindow::GetPosition() {
		int xpos, ypos;
		glfwGetWindowPos(m_Window, &xpos, &ypos);
		return Vec2{xpos, ypos};
	}
	uint32_t GLFWWindow::GetWindowWidth() {
		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		return width;
	}

	uint32_t GLFWWindow::GetWindowHeight() {
		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		return height;
	}

	Size2 GLFWWindow::GetWindowSize() {
		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}
	Rect<> GLFWWindow::GetWindowRect() {
		int xpos, ypos;
		glfwGetWindowPos(m_Window, &xpos, &ypos);
		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		return Rect<>{(Real)xpos,(Real)ypos,(Real)xpos+width, (Real)ypos+height};
	}

	uint32_t GLFWWindow::GetFramebufferWidth() {
		int width, height;
		glfwGetFramebufferSize(m_Window, &width, &height);
		return width;
	}

	uint32_t GLFWWindow::GetFramebufferHeight() {
		int width, height;
		glfwGetFramebufferSize(m_Window, &width, &height);
		return height;
	}

	Size2 GLFWWindow::GetFramebufferSize() {
		int width, height;
		glfwGetFramebufferSize(m_Window, &width, &height);
		return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}
} // namespace Imagine::Core
