//
// Created by Sayama on 29/04/2025.
//


#include <SDL3/SDL.h>

#ifdef MGN_RENDERER_VULKAN
#include <SDL3/SDL_vulkan.h>
#endif

#include "../Includes/Imagine/SDL3/SDL3Window.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include "Imagine/Rendering/MgnImGui.hpp"
#endif

using namespace Imagine::Core;

namespace Imagine::SDL3 {
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

	SDL3Mouse &SDL3Window::GetMouse() {
		return m_Mouse;
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

		auto state = SDL_GetMouseState(&m_Mouse.m_MousePosition.x, &m_Mouse.m_MousePosition.y);
		m_Mouse.m_ButtonStates[Mouse::Left]   = (state & SDL_BUTTON_LMASK)  ? SDL3Mouse::Down : SDL3Mouse::Up;
		m_Mouse.m_ButtonStates[Mouse::Middle] = (state & SDL_BUTTON_MMASK)  ? SDL3Mouse::Down : SDL3Mouse::Up;
		m_Mouse.m_ButtonStates[Mouse::Right]  = (state & SDL_BUTTON_RMASK)  ? SDL3Mouse::Down : SDL3Mouse::Up;
		m_Mouse.m_ButtonStates[Mouse::X1]     = (state & SDL_BUTTON_X1MASK) ? SDL3Mouse::Down : SDL3Mouse::Up;
		m_Mouse.m_ButtonStates[Mouse::X2]     = (state & SDL_BUTTON_X2MASK) ? SDL3Mouse::Down : SDL3Mouse::Up;
	}

	SDL3Window::~SDL3Window() {
		if (m_Window) {
			SDL_DestroyWindow(m_Window);
			m_Window = nullptr;
		}
	}

	void SDL3Window::Update() {
		for (int i = 0; i < m_Mouse.m_ButtonStates.size(); ++i) {
			m_Mouse.m_ButtonStates[i] &= ~(SDL3Mouse::Released | SDL3Mouse::Pressed);
		}

		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			bool canHandleEvent = true;
#ifdef MGN_IMGUI
			canHandleEvent = !ImGui_ImplSDL3_ProcessEvent(&e);
			// m_Mouse.m_MousePosition = ImGui::GetMousePos();
#endif
			// if (!canHandleEvent) continue;
			if (e.type == SDL_EVENT_QUIT) {
				m_ShouldClose = true;
			}
			else if (e.type == SDL_EVENT_WINDOW_MINIMIZED) {
				m_Minimized = true;
			}
			else if (e.type == SDL_EVENT_WINDOW_RESTORED) {
				m_Minimized = false;
			}
			else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				m_Mouse.m_ButtonStates[e.button.button] &= ~(SDL3Mouse::Released | SDL3Mouse::Up);
				m_Mouse.m_ButtonStates[e.button.button] |= SDL3Mouse::Pressed;
				m_Mouse.m_ButtonStates[e.button.button] |= SDL3Mouse::Down;
				m_Mouse.m_MousePosition = {e.button.x, e.button.y};
			}
			else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
				m_Mouse.m_ButtonStates[e.button.button] &= ~(SDL3Mouse::Pressed | SDL3Mouse::Down);
				m_Mouse.m_ButtonStates[e.button.button] |= SDL3Mouse::Released;
				m_Mouse.m_ButtonStates[e.button.button] |= SDL3Mouse::Up;
				m_Mouse.m_MousePosition = {e.button.x, e.button.y};
			}
			else if (e.type == SDL_EVENT_MOUSE_MOTION) {
				m_Mouse.m_MouseMovement = {e.motion.xrel, e.motion.yrel};
				m_Mouse.m_MousePosition = {e.motion.x, e.motion.y};
			}
		}
	}

	Vec2 SDL3Window::GetPosition() {
		int x, y;
		const bool result = SDL_GetWindowPosition(m_Window, &x, &y);
		MGN_CORE_ASSERT(result, "[SDL3] {}", SDL_GetError());
		return Vec2{x,y};
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
	Rect SDL3Window::GetWindowRect() {
		int x, y;
		{
			const bool result = SDL_GetWindowPosition(m_Window, &x, &y);
			MGN_CORE_ASSERT(result, "[SDL3] {}", SDL_GetError());
		}
		int width, height;
		{
			const bool result = SDL_GetWindowSize(m_Window, &width, &height);
			MGN_CORE_ASSERT(result, "[SDL3] {}", SDL_GetError());
		}
		return Rect{(float)x,(float)y,(float)x+width,(float)y+height};
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
} // namespace Imagine::SDL3
