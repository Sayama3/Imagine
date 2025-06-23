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
	Core::Mouse::ButtonState SDL3Mouse::GetButtonState(Button button) {
		return static_cast<ButtonState>(m_ButtonStates.at(button));
	}
	bool SDL3Mouse::IsButtonPressed(Button button) {
		return m_ButtonStates.at(button) & ButtonState::Pressed;
	}
	bool SDL3Mouse::IsButtonReleased(Button button) {
		return m_ButtonStates.at(button) & ButtonState::Released;
	}
	bool SDL3Mouse::IsButtonDown(Button button) {
		return m_ButtonStates.at(button) & ButtonState::Down;
	}
	bool SDL3Mouse::IsButtonUp(Button button) {
		return m_ButtonStates.at(button) & ButtonState::Up;
	}
} // namespace Imagine::SDL3
