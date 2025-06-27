//
// Created by Sayama on 29/04/2025.
//


#include <SDL3/SDL.h>

#ifdef MGN_RENDERER_VULKAN
#include <SDL3/SDL_vulkan.h>
#endif

#include "Imagine/SDL3/SDL3Window.hpp"

#include "Imagine/Events/ApplicationEvent.hpp"
#include "Imagine/Events/Event.hpp"
#include "Imagine/Events/KeyEvent.hpp"
#include "Imagine/Events/MouseEvent.hpp"

#include "Imagine/Rendering/Camera.hpp"
#include "Imagine/Rendering/Renderer.hpp"

#include "Imagine/Scene/Scene.hpp"
#include "Imagine/Scene/SceneManager.hpp"

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

#ifdef MGN_IMGUI
			ImGui_ImplSDL3_ProcessEvent(&e);
#endif

			if (e.type == SDL_EVENT_QUIT) {
				m_ShouldClose = true;
			}

			if (!m_EventCallBack) {
				continue;
			}

			if (e.type == SDL_EVENT_QUIT) {
				WindowCloseEvent event{};
				m_EventCallBack(event);
			}
			else if (e.type == SDL_EVENT_DROP_FILE) {
				WindowDropFileEvent event{1, &e.drop.data};
				m_EventCallBack(event);
			}
			else if (e.type == SDL_EVENT_WINDOW_RESIZED) {
				WindowResizeEvent event{static_cast<uint32_t>(e.window.data1), static_cast<uint32_t>(e.window.data2)};
				m_EventCallBack(event);
			}
			else if (e.type == SDL_EVENT_WINDOW_MINIMIZED) {
				m_Minimized = true;
				WindowMinifyEvent event{true};
				m_EventCallBack(event);
			}
			else if (e.type == SDL_EVENT_WINDOW_RESTORED) {
				m_Minimized = false;
				WindowMinifyEvent event{false};
				m_EventCallBack(event);
			}
			else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				MouseButtonPressedEvent event{GetMouseButton(e.button.button)};
				m_EventCallBack(event);
			}
			else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
				MouseButtonReleasedEvent event{GetMouseButton(e.button.button)};
				m_EventCallBack(event);
			}
			else if (e.type == SDL_EVENT_MOUSE_MOTION) {
				MouseMovedEvent event{e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel};
				m_EventCallBack(event);
			}
			else if (e.type == SDL_EVENT_MOUSE_WHEEL) {
				MouseScrolledEvent event{e.wheel.x, e.wheel.y};
				m_EventCallBack(event);
			}
			else if (e.type == SDL_EVENT_KEY_DOWN) {
				KeyPressedEvent event{GetKeyCode(e.key.scancode), e.key.repeat ? 1 : 0};
				m_EventCallBack(event);
			}
			else if (e.type == SDL_EVENT_KEY_UP) {
				KeyReleasedEvent event{GetKeyCode(e.key.scancode)};
				m_EventCallBack(event);
			}
			else if (e.type == SDL_EVENT_TEXT_INPUT) {
				TextTypedEvent event{e.text.text};
				m_EventCallBack(event);
			}
		}
	}

	void SDL3Window::SetEventCallback(const EventCallBackFn &callback) {
		m_EventCallBack = callback;
	}

	Vec2 SDL3Window::GetPosition() {
		int x, y;
		const bool result = SDL_GetWindowPosition(m_Window, &x, &y);
		MGN_CORE_ASSERT(result, "[SDL3] {}", SDL_GetError());
		return Vec2{x, y};
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
	Rect<> SDL3Window::GetWindowRect() {
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
		return Rect<>{(float) x, (float) y, (float) x + width, (float) y + height};
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
