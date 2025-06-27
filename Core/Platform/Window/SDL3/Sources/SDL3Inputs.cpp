//
// Created by Iannis on 23/06/2025.
//

#include "Imagine/SDL3/SDL3Inputs.hpp"

namespace Imagine::SDL3 {
	Vec2 SDL3Mouse::GetPosition() {
		return m_MousePosition;
	}
	Vec2 SDL3Mouse::GetMovement() {
		return m_MouseMovement;
	}
	Core::MouseInput::ButtonState SDL3Mouse::GetButtonState(Imagine::Mouse button) {
		return static_cast<ButtonState>(m_ButtonStates.at(static_cast<int32_t>(button)));
	}
	bool SDL3Mouse::IsButtonPressed(Imagine::Mouse button) {
		return m_ButtonStates.at(static_cast<int32_t>(button)) & ButtonState::Pressed;
	}
	bool SDL3Mouse::IsButtonReleased(Imagine::Mouse button) {
		return m_ButtonStates.at(static_cast<int32_t>(button)) & ButtonState::Released;
	}
	bool SDL3Mouse::IsButtonDown(Imagine::Mouse button) {
		return m_ButtonStates.at(static_cast<int32_t>(button)) & ButtonState::Down;
	}
	bool SDL3Mouse::IsButtonUp(Imagine::Mouse button) {
		return m_ButtonStates.at(static_cast<int32_t>(button)) & ButtonState::Up;
	}
} // namespace Imagine::SDL3
