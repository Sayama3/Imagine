//
// Created by Sayama on 29/04/2025.
//

#include "Imagine/Application/Window.hpp"

#if defined(MGN_WINDOW_GLFW)
#include "GLFWWindow.hpp"
#elif defined(MGN_WINDOW_SDL3)
#include "SDL3Window.hpp"
#else
#include "HeadLessWindow.hpp"
#endif

namespace Imagine::Core {

	Window *Window::s_Window{nullptr};

	Window *Window::Create(const std::string &windowName, WindowParameters parameters) {
		Window *window{nullptr};
#if defined(MGN_WINDOW_GLFW)
		window = new GLFWWindow(windowName, std::move(parameters));
#elif defined(MGN_WINDOW_SDL3)
		window = new SDL3Window(windowName, std::move(parameters));
#else
		window = new HeadLessWindow(windowName, std::move(parameters));
#endif
		return window;
	}

	Window *Window::Initialize(const std::string &windowName, WindowParameters parameters) {
		if (s_Window) return s_Window;
		s_Window = Window::Create(windowName, std::move(parameters));
		return s_Window;
	}

	void Window::Shutdown() {
		delete s_Window;
		s_Window = nullptr;
	}

	Window *Window::Get() {
		return s_Window;
	}
} // namespace Imagine::Core
