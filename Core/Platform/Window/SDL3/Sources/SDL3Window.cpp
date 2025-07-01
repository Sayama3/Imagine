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
#include "Imagine/SDL3/SDL3Inputs.hpp"

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

	const Core::KeyboardState &SDL3Window::GetKeyboardState() const {
		return m_Keyboard;
	}

	const Core::MouseState &SDL3Window::GetMouseState() const {
		return m_Mouse;
	}

	bool SDL3Window::OnPixelSizeChanged(void *userdata, SDL_Event *e) {
		SDL3Window *sdlWindow = static_cast<SDL3Window *>(userdata);
		if (!sdlWindow->m_EventCallBack) return true;
		switch (e->type) {
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:{
#ifdef MGN_IMGUI
				ImGui_ImplSDL3_ProcessEvent(e);
#endif
				WindowResizeEvent event{static_cast<uint32_t>(e->window.data1), static_cast<uint32_t>(e->window.data2)};
				sdlWindow->m_EventCallBack(event, false);
			} break;
			default:
				break;
		}

		return true;
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

// #if false
		SDL_SetEventFilter(SDL3Window::OnPixelSizeChanged, this);
// #endif
		//Initialze Inputs
		SDL_MouseButtonFlags buttons = SDL_GetMouseState(&m_Mouse.x, &m_Mouse.y);
		m_Mouse[Mouse::Button1] = buttons & SDL_BUTTON_LMASK ? Button::DOWN : Button::UP;
		m_Mouse[Mouse::Button2] = buttons & SDL_BUTTON_MMASK ? Button::DOWN : Button::UP;
		m_Mouse[Mouse::Button3] = buttons & SDL_BUTTON_RMASK ? Button::DOWN : Button::UP;
		m_Mouse[Mouse::Button4] = buttons & SDL_BUTTON_X1MASK ? Button::DOWN : Button::UP;
		m_Mouse[Mouse::Button5] = buttons & SDL_BUTTON_X2MASK ? Button::DOWN : Button::UP;
		m_Mouse.WindowId = SDL_GetWindowID(SDL_GetMouseFocus());

		int keyCount;
		const bool* keysPressed = SDL_GetKeyboardState(&keyCount);
		for (int key = 0; key < keyCount; ++key) {
			const Key k = GetKeyCode(static_cast<SDL_Scancode>(key));
			if (k == Key::Unknown) continue;
			m_Keyboard[k] = keysPressed[key] ? Button::DOWN : Button::UP;
		}
		m_Keyboard.WindowId = SDL_GetWindowID(SDL_GetKeyboardFocus());
	}

	SDL3Window::~SDL3Window() {
		if (m_Window) {
			SDL_DestroyWindow(m_Window);
			m_Window = nullptr;
		}
	}

	void SDL3Window::Update() {

		// Reset the PRESSED & RELEASED that should only be for a frame
		for (Core::Button & button: m_Mouse.Buttons) {
			button &= ~(Button::PRESSED|Button::RELEASED);
		}
		for (Core::Button & key: m_Keyboard.Keys) {
			key &= ~(Button::PRESSED|Button::RELEASED);
		}

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

			switch (e.type) {
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				case SDL_EVENT_QUIT: {
					WindowCloseEvent event{};
					m_EventCallBack(event, true);
				} break;
				case SDL_EVENT_DROP_FILE: {
					WindowDropFileEvent event{1, &e.drop.data};
					m_EventCallBack(event, true);
				} break;
				// case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
				case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
				case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
				// case SDL_EVENT_WINDOW_RESIZED:
				{
					WindowResizeEvent event{static_cast<uint32_t>(e.window.data1), static_cast<uint32_t>(e.window.data2)};
					m_EventCallBack(event, true);
				} break;
				case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: {
					// TODO: Handle Fullscreen mode.
				} break;
				case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: {
					// TODO: Handle Fullscreen mode.
				} break;
				case SDL_EVENT_WINDOW_MINIMIZED: {
					m_Minimized = true;
					WindowMinifyEvent event{true};
					m_EventCallBack(event, true);
				} break;
				case SDL_EVENT_WINDOW_RESTORED: {
					m_Minimized = false;
					WindowMinifyEvent event{false};
					m_EventCallBack(event, true);
				} break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN: {
					const auto mouseButton = GetMouseButton(e.button.button);
					m_Mouse[mouseButton] = (Button::DOWN | Button::PRESSED);
					m_Mouse.WindowId = e.button.windowID;
					MouseButtonPressedEvent event{mouseButton};
					m_EventCallBack(event, true);
				} break;
				case SDL_EVENT_MOUSE_BUTTON_UP: {
					const auto mouseButton = GetMouseButton(e.button.button);
					m_Mouse[mouseButton] = (Button::UP | Button::RELEASED);
					m_Mouse.WindowId = e.button.windowID;
					MouseButtonReleasedEvent event{mouseButton};
					m_EventCallBack(event, true);
				} break;
				case SDL_EVENT_MOUSE_MOTION: {
					MouseMovedEvent event{e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel};
					m_Mouse.X = e.motion.x;
					m_Mouse.Y =  e.motion.y;

					m_Mouse.MotionX = e.motion.xrel;
					m_Mouse.MotionY =  e.motion.yrel;

					m_Mouse.WindowId = e.motion.windowID;
					m_EventCallBack(event, true);
				} break;
				case SDL_EVENT_MOUSE_WHEEL: {
					MouseScrolledEvent event{e.wheel.x, e.wheel.y};
					m_Mouse.WheelX = e.wheel.x;
					m_Mouse.WheelY = e.wheel.y;
					m_Mouse.WindowId = e.wheel.windowID;
					m_EventCallBack(event, true);
				} break;
				case SDL_EVENT_KEY_DOWN: {
					const Key key = GetKeyCode(e.key.scancode);
					if (key == Key::Unknown) continue;
					if (!e.key.repeat) m_Keyboard[key] = (Button::DOWN|Button::PRESSED);
					m_Keyboard.WindowId = e.key.windowID;
					KeyPressedEvent event{key, e.key.repeat ? 1 : 0};
					m_EventCallBack(event, true);
				} break;
				case SDL_EVENT_KEY_UP: {
					const Key key = GetKeyCode(e.key.scancode);
					if (key == Key::Unknown) continue;
					m_Keyboard[key] = (Button::UP|Button::RELEASED);
					m_Keyboard.WindowId = e.key.windowID;
					KeyReleasedEvent event{key};
					m_EventCallBack(event, true);
				} break;
				case SDL_EVENT_TEXT_INPUT: {
					TextTypedEvent event{e.text.text};
					m_EventCallBack(event, true);
				} break;

				default: {
					break;
				}
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
