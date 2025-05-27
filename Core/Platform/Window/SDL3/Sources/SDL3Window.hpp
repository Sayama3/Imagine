#pragma once

#include "Imagine/Application/Window.hpp"

namespace Imagine::Core {
	class SDL3Window final : public Window {
	public:
		SDL3Window(const std::string& windowName, const WindowParameters parameters);
		~SDL3Window() override;

		void Update() override;

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
		struct SDL_Window* m_Window{nullptr};
		bool frameBufferResized = false;
		bool m_ShouldClose= false;
		bool m_Minimized = false;
	};
}
