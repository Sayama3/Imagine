//
// Created by Iannis on 23/06/2025.
//

#include "Imagine/Core/Inputs.hpp"

#include "Imagine/Application/Window.hpp"
#include "Imagine/Application/WindowParameters.hpp"

#if defined(MGN_WINDOW_SDL3)
#include "Imagine/SDL3/SDL3Window.hpp"
#elif defined(MGN_WINDOW_GLFW)
#include "GLFWWindow.hpp"
#endif

namespace Imagine::Core {
	Mouse &Inputs::GetMouse() {
#if defined(MGN_WINDOW_SDL3)
		return reinterpret_cast<SDL3::SDL3Window*>(Window::Get())->GetMouse();
#elif defined(MGN_WINDOW_GLFW)
		return reinterpret_cast<Imagine::GLFW::GLFWWindow*>(Window::Get())->GetMouse();
#else
#error "No valid window found."
#endif
	}
} // namespace Imagine::Core
