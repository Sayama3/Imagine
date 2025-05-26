#pragma once

#include "Imagine/Application/Window.hpp"

namespace Imagine::Core {
	class SDL3Window : public Window {
	public:
		SDL3Window(const std::string& windowName, const WindowParameters parameters);
		~SDL3Window() override;

		void Update() override;

		uint32_t GetWindowWidth() override;
		uint32_t GetWindowHeight() override;

		uint32_t GetFramebufferWidth() override;
		uint32_t GetFramebufferHeight() override;
		void * GetNativeWindow() override;

		bool WindowHasResized() override;
		bool ShouldClose() override;
	private:
		static void framebufferResizeCallback(void* window, int width, int height);

	private:
		void* m_Window;
		bool frameBufferResized = false;
	};
}
