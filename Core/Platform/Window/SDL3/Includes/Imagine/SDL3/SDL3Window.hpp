#pragma once

#include "Imagine/Application/Window.hpp"
#include "Imagine/SDL3/SDL3Inputs.hpp"


namespace Imagine::SDL3 {
	class SDL3Window final : public Core::Window {
	public:
		SDL3Window(const std::string &windowName, const Core::WindowParameters parameters);
		~SDL3Window() override;

		void Update() override;
		void SendImGuiCommands() override {}

		uint32_t GetWindowWidth() override;
		uint32_t GetWindowHeight() override;
		Core::Size2 GetWindowSize() override;

		uint32_t GetFramebufferWidth() override;
		uint32_t GetFramebufferHeight() override;
		Core::Size2 GetFramebufferSize() override;
		void *GetNativeWindow() override;

		bool IsMinimized() override;
		bool ShouldClose() override;

		void *GetWindowPtr() override;
	public:
		SDL3Mouse& GetMouse();
	private:
		static void framebufferResizeCallback(void *window, int width, int height);

	private:
		struct SDL_Window *m_Window{nullptr};
		bool m_ShouldClose = false;
		bool m_Minimized = false;
		SDL3Mouse m_Mouse;
	};
} // namespace Imagine::Core
