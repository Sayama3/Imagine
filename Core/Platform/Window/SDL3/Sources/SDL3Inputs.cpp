//
// Created by Iannis on 23/06/2025.
//

#include "Imagine/SDL3/SDL3Inputs.hpp"

#include "Imagine/SDL3/SDL3Window.hpp"

namespace Imagine {

	const Core::KeyboardState& Core::Inputs::GetKeyboardState() {
		return static_cast<SDL3::SDL3Window*>(Core::Window::Get())->GetKeyboardState();
	}
	const Core::MouseState& Core::Inputs::GetMouseState() {
		return static_cast<SDL3::SDL3Window*>(Core::Window::Get())->GetMouseState();
	}
} // namespace Imagine::SDL3
