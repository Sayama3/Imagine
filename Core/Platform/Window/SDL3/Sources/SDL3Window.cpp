//
// Created by Sayama on 29/04/2025.
//


#include <SDL3/SDL.h>

#ifdef MGN_RENDERER_VULKAN
#include <SDL3/SDL_vulkan.h>
#endif

#include "SDL3Window.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include "Imagine/Rendering/MgnImGui.hpp"
#endif

namespace Imagine::Core {
	void *SDL3Window::GetNativeWindow() {
		return m_Window;
	}
	bool SDL3Window::IsMinimized() {
		return m_Minimized;
	}

	bool SDL3Window::ShouldClose() {
		return m_ShouldClose;
	}

	void *SDL3Window::GetWindowPtr() {
		return m_Window;
	}

	void SDL3Window::framebufferResizeCallback(void *window, int width, int height) {
	}

	SDL3Window::SDL3Window(const std::string &windowName, const WindowParameters parameters) :
		Window{} {

		// We initialize SDL and create a window with it.
		SDL_Init(SDL_INIT_VIDEO);

		SDL_WindowFlags window_flags{};

		if (parameters.Resizable) window_flags |= SDL_WINDOW_RESIZABLE;

#ifdef MGN_RENDERER_VULKAN
		window_flags |= (SDL_WindowFlags) (SDL_WINDOW_VULKAN);
#endif

		// TODO: Use `parameters.Resizable`.
		m_Window = SDL_CreateWindow(windowName.c_str(), parameters.Width, parameters.Height, window_flags);
	}

	SDL3Window::~SDL3Window() {
		if (m_Window) {
			SDL_DestroyWindow(m_Window);
			m_Window = nullptr;
		}
	}

	void SDL3Window::Update() {
		SDL_Event e;

		while (SDL_PollEvent(&e) != 0) {
			bool canHandleEvent = true;
#ifdef MGN_IMGUI
			canHandleEvent = !ImGui_ImplSDL3_ProcessEvent(&e);
#endif
			if (!canHandleEvent) continue;

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
		SDL_GetWindowSize(m_Window, &width, nullptr);
		return width;
	}

	uint32_t SDL3Window::GetWindowHeight() {
		int height;
		SDL_GetWindowSize(m_Window, nullptr, &height);
		return height;
	}

	Size2 SDL3Window::GetWindowSize() {
		int width, height;
		SDL_GetWindowSize(m_Window, &width, &height);
		return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}

	uint32_t SDL3Window::GetFramebufferWidth() {
		int width;
		SDL_GetWindowSizeInPixels(m_Window, &width, nullptr);
		return width;
	}

	uint32_t SDL3Window::GetFramebufferHeight() {
		int height;
		SDL_GetWindowSizeInPixels(m_Window, nullptr, &height);
		return height;
	}

	Size2 SDL3Window::GetFramebufferSize() {
		int width, height;
		SDL_GetWindowSizeInPixels(m_Window, &width, &height);
		return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}
} // namespace Imagine::Core
