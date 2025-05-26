//
// Created by Sayama on 29/04/2025.
//


#include <SDL3/SDL.h>

#ifdef MGN_RENDERER_VULKAN
#include <SDL3/SDL_vulkan.h>
#endif

#include "SDL3Window.hpp"

namespace Imagine::Core {
	void * SDL3Window::GetNativeWindow() {
		return m_Window;
	}

	bool SDL3Window::WindowHasResized() {
		return frameBufferResized;
	}

	bool SDL3Window::ShouldClose() {
		return m_ShouldClose;
	}

	void SDL3Window::framebufferResizeCallback(void* window, int width, int height) {
	}

	SDL3Window::SDL3Window(const std::string& windowName, const WindowParameters parameters) : Window{} {

		// We initialize SDL and create a window with it.
		SDL_Init(SDL_INIT_VIDEO);

		SDL_WindowFlags window_flags;

#ifdef MGN_RENDERER_VULKAN
		window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
#endif

		//TODO: Use `parameters.Resizable`.
		m_Window = SDL_CreateWindow(windowName.c_str(), parameters.Width, parameters.Height, window_flags);

	}

	SDL3Window::~SDL3Window() {
		if (m_Window)
		{
			SDL_DestroyWindow(m_Window);
			m_Window = nullptr;
		}
	}

	void SDL3Window::Update() {
		SDL_Event e;

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_EVENT_QUIT) {
				m_ShouldClose = true;
			}
			else if (e.type == SDL_EVENT_WINDOW_MINIMIZED) {
				m_Minimized = true;
			}
			else if (e.type == SDL_EVENT_WINDOW_RESTORED) {
				m_Minimized = false;
			}
		}
	}

	uint32_t SDL3Window::GetWindowWidth() {
		int width;
		SDL_GetWindowSize(m_Window, &width,nullptr);
		return width;
	}

	uint32_t SDL3Window::GetWindowHeight() {
		int height;
		SDL_GetWindowSize(m_Window, nullptr,&height);
		return height;
	}

	uint32_t SDL3Window::GetFramebufferWidth() {
		int width;
		SDL_GetWindowSizeInPixels(m_Window, &width,nullptr);
		return width;
	}

	uint32_t SDL3Window::GetFramebufferHeight() {
		int height;
		SDL_GetWindowSizeInPixels(m_Window, nullptr,&height);
		return height;
	}
}
