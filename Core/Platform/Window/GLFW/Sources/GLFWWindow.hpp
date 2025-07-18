#pragma once


#include "Imagine/Application/Window.hpp"
#include "Imagine/Core/Inputs.hpp"

struct GLFWwindow;

namespace Imagine::GLFW {
	class GLFWWindow : public Window {
	public:
		GLFWWindow(const std::string &windowName, const WindowParameters parameters);
		~GLFWWindow() override;

		void Update() override;
		void SendImGuiCommands() override {}

		Vec2 GetPosition() override;

		uint32_t GetWindowWidth() override;
		uint32_t GetWindowHeight() override;
		Size2 GetWindowSize() override;

		virtual Rect<> GetWindowRect() override;

		uint32_t GetFramebufferWidth() override;
		uint32_t GetFramebufferHeight() override;
		Size2 GetFramebufferSize() override;

		void *GetNativeWindow() override;

		bool ShouldClose() override;

		void *GetWindowPtr() override;
		MouseInput & GetMouse();

	private:
		static void framebufferResizeCallback(void *window, int width, int height);

	public:
		virtual bool IsMinimized() override;

	private:
		struct GLFWwindow *m_Window{nullptr};
		bool frameBufferResized = false;
	};
} // namespace Imagine
