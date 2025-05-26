//
// Created by Sayama on 29/04/2025.
//

#include <SDL3/SDL.h>

#include "SDL3Window.hpp"

namespace Imagine::Core {
	void * SDL3Window::GetNativeWindow() {
		return m_Window;
	}

	bool SDL3Window::WindowHasResized() {
		return frameBufferResized;
	}

	bool SDL3Window::ShouldClose() {
		return true;
	}

	void SDL3Window::framebufferResizeCallback(void* window, int width, int height) {
	}

	SDL3Window::SDL3Window(const std::string& windowName, const WindowParameters parameters) : Window{} {
	}

	SDL3Window::~SDL3Window() {
	}

	void SDL3Window::Update() {
	}

	uint32_t SDL3Window::GetWindowWidth() {
		return 0;
	}

	uint32_t SDL3Window::GetWindowHeight() {
		return 0;
	}

	uint32_t SDL3Window::GetFramebufferWidth() {
		return 0;
	}

	uint32_t SDL3Window::GetFramebufferHeight() {
		return 0;
	}
}
