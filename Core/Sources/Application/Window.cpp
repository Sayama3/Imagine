//
// Created by Sayama on 29/04/2025.
//

#include "Imagine/Application/Window.hpp"

#ifdef MGN_WINDOW_GLFW
#include "GLFWWindow.hpp"
#else
#include "HeadLessWindow.hpp"
#endif

namespace Imagine::Core {
	Window* Window::CreateWindow(const std::string &windowName, WindowParameters parameters) {
#ifdef MGN_WINDOW_GLFW
			return new GLFWWindow(windowName, std::move(parameters));
#else
			return new HeadLessWindow(windowName, std::move(parameters));
#endif
	}

}
