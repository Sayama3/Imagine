//
// Created by Iannis on 23/06/2025.
//

#include "Imagine/SDL3/SDL3Inputs.hpp"

#include "Imagine/SDL3/SDL3Window.hpp"

namespace Imagine {

	const KeyboardState& Inputs::GetKeyboardState() {
		return static_cast<SDL3::SDL3Window*>(Window::Get())->GetKeyboardState();
	}
	const MouseState& Inputs::GetMouseState() {
		return static_cast<SDL3::SDL3Window*>(Window::Get())->GetMouseState();
	}
} // namespace Imagine::SDL3
