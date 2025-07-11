//
// Created by Iannis on 23/06/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/KeyCodes.hpp"
#include "Imagine/Core/MouseButtonCodes.hpp"

namespace Imagine {

	struct Button {
		enum ButtonState : uint8_t {
			PRESSED = BIT(0),
			RELEASED = BIT(1),
			DOWN = BIT(2),
			UP = BIT(3),
		};

		bool operator==(const Button & o) const {
			return state == o.state;
		}
		bool operator!=(const Button & o) const {
			return state != o.state;
		}
		auto operator<=>(const Button & o) const {
			return state <=> o.state;
		}

		Button& operator=(uint8_t s) {
			state = (ButtonState)s;
			return *this;
		}
		Button& operator=(ButtonState s) {
			state = s;
			return *this;
		}

		Button& operator|=(const uint8_t s) {
			(*((uint8_t*)&state)) |= s;
			return *this;
		}
		Button& operator&=(const uint8_t s) {
			(*((uint8_t*)&state)) &= s;
			return *this;
		}
		Button& operator|=(const ButtonState s) {
			*this |= (uint8_t)s;
			return *this;
		}
		Button& operator&=(const ButtonState s) {
			*this &= (uint8_t)s;
			return *this;
		}
		Button& operator|=(const Button s) {
			*this |= (uint8_t)s.state;
			return *this;
		}
		Button& operator&=(const Button s) {
			*this &= (uint8_t)s.state;
			return *this;
		}
		Button operator&(const uint8_t s) {
			Button o(*this);
			o &= s;
			return o;
		}
		Button operator|(const uint8_t s) {
			Button o(*this);
			o |= s;
			return o;
		}
		Button operator&(const ButtonState s) {
			Button o(*this);
			o &= s;
			return o;
		}
		Button operator|(const ButtonState s) {
			Button o(*this);
			o |= s;
			return o;
		}
		Button operator&(const Button s) {
			Button o(*this);
			o &= s;
			return o;
		}
		Button operator|(const Button s) {
			Button o(*this);
			o |= s;
			return o;
		}

		ButtonState state;

		bool IsPressed() const {return state & PRESSED != 0;}
		bool IsReleased() const {return state & RELEASED != 0;}
		bool IsDown() const {return state & DOWN != 0;}
		bool IsUp() const {return state & UP != 0;}
	};

	struct KeyboardState {
		static constexpr int32_t KeyCount{MGN_KEY_COUNT};
		std::array<Button, KeyCount> Keys{Button::UP};
		uint32_t WindowId = 0;

		Button& operator[](const uint64_t i) {return Keys[i];}
		const Button& operator[](const uint64_t i) const {return Keys[i];}

		Button& operator[](const Key i) {return Keys[(int32_t)i];}
		const Button& operator[](const Key i) const {return Keys[(int32_t)i];}
	};

	struct MouseState {
		MouseState() = default;
		~MouseState() = default;

		static constexpr int32_t ButtonCount{static_cast<int32_t>(Mouse::Last) + 1};
		std::array<Button, ButtonCount> Buttons{Button::UP};
		union {
			struct {float X, Y;};
			struct {float x, y;};
		};
		float MotionX = 0, MotionY = 0;
		float WheelX = 0, WheelY = 0;
		uint32_t WindowId = 0;

		Button& operator[](const uint64_t i) {return Buttons[i];}
		const Button& operator[](const uint64_t i) const {return Buttons[i];}
		Button& operator[](const Mouse i) {return Buttons[(int32_t)i];}
		const Button& operator[](const Mouse i) const {return Buttons[(int32_t)i];}
	};

	class Inputs {
		static const KeyboardState& GetKeyboardState();
		static const MouseState& GetMouseState();
	};

} // namespace Imagine
