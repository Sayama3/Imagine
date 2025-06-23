#pragma once


#include "Imagine/Application/Window.hpp"

struct GLFWwindow;

namespace Imagine::GLFW {
	class GLFWWindow : public Core::Window {
	public:
		GLFWWindow(const std::string &windowName, const Core::WindowParameters parameters);
		~GLFWWindow() override;

		void Update() override;
		void SendImGuiCommands() override {}

		uint32_t GetWindowWidth() override;
		uint32_t GetWindowHeight() override;
		Core::Size2 GetWindowSize() override;

		uint32_t GetFramebufferWidth() override;
		uint32_t GetFramebufferHeight() override;
		Core::Size2 GetFramebufferSize() override;

		void *GetNativeWindow() override;

		bool ShouldClose() override;

		void *GetWindowPtr() override;

	private:
		static void framebufferResizeCallback(void *window, int width, int height);

	private:
		struct GLFWwindow *m_Window{nullptr};
		bool frameBufferResized = false;
	};
} // namespace Imagine::Core
