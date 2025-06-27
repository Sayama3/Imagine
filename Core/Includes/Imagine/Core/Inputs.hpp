//
// Created by Iannis on 23/06/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/Math.hpp"

namespace Imagine::Core {

	class Mouse {
	public:
		//TODO: Change the buttons for the 'Imagine/Core/MouseButtonCodes.hpp'.

		enum Button : uint8_t {
			Left = 1,
			Middle = 2,
			Right = 3,
			X1 = 4,
			X2 = 5,
		};

		enum ButtonState : uint8_t {
			None = 0,
			Pressed = BIT(0),
			Released = BIT(1),
			Down = BIT(2),
			Up = BIT(3),
		};
	public:
		virtual ~Mouse() = default;

	public:
		virtual Vec2 GetPosition() = 0;
		virtual Vec2 GetMovement() = 0;

		virtual ButtonState GetButtonState(Button button) = 0;
		virtual bool IsButtonPressed(Button button) = 0;
		virtual bool IsButtonReleased(Button button) = 0;
		virtual bool IsButtonDown(Button button) = 0;
		virtual bool IsButtonUp(Button button) = 0;
	};

	class Inputs {
	public:
		static Mouse& GetMouse();
	};

} // namespace Imagine::Core
