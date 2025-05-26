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
	Window* Window::Create(const std::string &windowName, WindowParameters parameters) {
#if defined(MGN_WINDOW_GLFW)
			return new GLFWWindow(windowName, std::move(parameters));
#elif defined(MGN_WINDOW_SDL3)
			return new SDL3Window(windowName, std::move(parameters));
#else
			return new HeadLessWindow(windowName, std::move(parameters));
#endif
	}

}
