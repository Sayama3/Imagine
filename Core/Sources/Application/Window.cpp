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

	Window* Window::s_Window{nullptr};

	Window* Window::Initialize(const std::string &windowName, WindowParameters parameters) {
			if (s_Window) return s_Window;
#if defined(MGN_WINDOW_GLFW)
			s_Window = new GLFWWindow(windowName, std::move(parameters));
#elif defined(MGN_WINDOW_SDL3)
			s_Window = new SDL3Window(windowName, std::move(parameters));
#else
			s_Window = new HeadLessWindow(windowName, std::move(parameters));
#endif
		return s_Window;
	}

	void Window::Shutdown()
	{
		delete s_Window;
		s_Window = nullptr;
	}

	Window* Window::Get()
	{
		return s_Window;
	}
}
