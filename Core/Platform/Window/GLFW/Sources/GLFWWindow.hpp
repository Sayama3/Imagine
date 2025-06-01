#pragma once

#include <GLFW/glfw3.h>

#include "Imagine/Application/Window.hpp"

namespace Imagine::Core {
	class GLFWWindow : public Window {
	public:
		GLFWWindow(const std::string& windowName, const WindowParameters parameters);
		~GLFWWindow() override;

		void Update() override;
		void SendImGuiCommands() override {}

		uint32_t GetWindowWidth() override;
		uint32_t GetWindowHeight() override;
		Size2 GetWindowSize() override;

		uint32_t GetFramebufferWidth() override;
		uint32_t GetFramebufferHeight() override;
		Size2 GetFramebufferSize() override;

		void * GetNativeWindow() override;

		bool WindowHasResized() override;
		bool ShouldClose() override;

		void* GetWindowPtr() override;
	private:
		static void framebufferResizeCallback(void* window, int width, int height);

	private:
		struct GLFWwindow* m_Window{nullptr}; //TODO: Change the window type to GLFW
		bool frameBufferResized = false;
	};
}
