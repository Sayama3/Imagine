//
// Created by Iannis on 23/06/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/MouseButtonCodes.hpp"

namespace Imagine::Core {

	class MouseInput {
	public:
		//TODO: Change the buttons for the 'Imagine/Core/MouseButtonCodes.hpp'.

		enum ButtonState : uint8_t {
			None = 0,
			Pressed = BIT(0),
			Released = BIT(1),
			Down = BIT(2),
			Up = BIT(3),
		};
	public:
		virtual ~MouseInput() = default;

	public:
		virtual Vec2 GetPosition() = 0;
		virtual Vec2 GetMovement() = 0;

		virtual ButtonState GetButtonState(Imagine::Mouse button) = 0;
		virtual bool IsButtonPressed(Imagine::Mouse button) = 0;
		virtual bool IsButtonReleased(Imagine::Mouse button) = 0;
		virtual bool IsButtonDown(Imagine::Mouse button) = 0;
		virtual bool IsButtonUp(Imagine::Mouse button) = 0;
	};

	class Inputs {
	public:
		static MouseInput& GetMouse();
	};

} // namespace Imagine::Core
