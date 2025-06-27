//
// Created by Iannis on 23/06/2025.
//

#pragma once
#include <SDL3/SDL.h>
#include "Imagine/Core/Inputs.hpp"

#include "Imagine/Core/KeyCodes.hpp"
#include "Imagine/Core/MouseButtonCodes.hpp"

namespace Imagine {
namespace SDL3 {

	class SDL3Mouse final : public Core::MouseInput {
	public:
	public:
		SDL3Mouse() = default;
		~SDL3Mouse() = default;
	public:
		virtual Vec2 GetPosition() override;
		virtual Vec2 GetMovement() override;
		virtual ButtonState GetButtonState(Imagine::Mouse button) override;
		virtual bool IsButtonPressed(Imagine::Mouse button) override;
		virtual bool IsButtonReleased(Imagine::Mouse button) override;
		virtual bool IsButtonDown(Imagine::Mouse button) override;
		virtual bool IsButtonUp(Imagine::Mouse button) override;
	public:
		Vec2 m_MousePosition{0};
		Vec2 m_MouseMovement{0};
		std::array<uint8_t, 12> m_ButtonStates;
	};

	class SDL3Inputs {
	};

	inline Imagine::Mouse GetMouseButton(const SDL_MouseButtonFlags mb) {
		switch (mb) {
			case SDL_BUTTON_LEFT:
				return Imagine::Mouse::Left;
			case SDL_BUTTON_MIDDLE:
				return Imagine::Mouse::Middle;
			case SDL_BUTTON_RIGHT:
				return Imagine::Mouse::Right;
			case SDL_BUTTON_X1:
				return Imagine::Mouse::Button4;
			case SDL_BUTTON_X2:
				return Imagine::Mouse::Button5;
			default:
				return Imagine::Mouse(mb-1);
		}
	}

	inline Imagine::Key GetKeyCode(const SDL_Scancode sc) {
		switch (sc) {
			case SDL_SCANCODE_UNKNOWN:
				return (Imagine::Key)MGN_KEY_UNKNOWN;
				break;
			case SDL_SCANCODE_A:
				return (Imagine::Key)MGN_KEY_A;
				break;
			case SDL_SCANCODE_B:
				return (Imagine::Key)MGN_KEY_B;
				break;
			case SDL_SCANCODE_C:
				return (Imagine::Key)MGN_KEY_C;
				break;
			case SDL_SCANCODE_D:
				return (Imagine::Key)MGN_KEY_D;
				break;
			case SDL_SCANCODE_E:
				return (Imagine::Key)MGN_KEY_E;
				break;
			case SDL_SCANCODE_F:
				return (Imagine::Key)MGN_KEY_F;
				break;
			case SDL_SCANCODE_G:
				return (Imagine::Key)MGN_KEY_G;
				break;
			case SDL_SCANCODE_H:
				return (Imagine::Key)MGN_KEY_H;
				break;
			case SDL_SCANCODE_I:
				return (Imagine::Key)MGN_KEY_I;
				break;
			case SDL_SCANCODE_J:
				return (Imagine::Key)MGN_KEY_J;
				break;
			case SDL_SCANCODE_K:
				return (Imagine::Key)MGN_KEY_K;
				break;
			case SDL_SCANCODE_L:
				return (Imagine::Key)MGN_KEY_L;
				break;
			case SDL_SCANCODE_M:
				return (Imagine::Key)MGN_KEY_M;
				break;
			case SDL_SCANCODE_N:
				return (Imagine::Key)MGN_KEY_N;
				break;
			case SDL_SCANCODE_O:
				return (Imagine::Key)MGN_KEY_O;
				break;
			case SDL_SCANCODE_P:
				return (Imagine::Key)MGN_KEY_P;
				break;
			case SDL_SCANCODE_Q:
				return (Imagine::Key)MGN_KEY_Q;
				break;
			case SDL_SCANCODE_R:
				return (Imagine::Key)MGN_KEY_R;
				break;
			case SDL_SCANCODE_S:
				return (Imagine::Key)MGN_KEY_S;
				break;
			case SDL_SCANCODE_T:
				return (Imagine::Key)MGN_KEY_T;
				break;
			case SDL_SCANCODE_U:
				return (Imagine::Key)MGN_KEY_U;
				break;
			case SDL_SCANCODE_V:
				return (Imagine::Key)MGN_KEY_V;
				break;
			case SDL_SCANCODE_W:
				return (Imagine::Key)MGN_KEY_W;
				break;
			case SDL_SCANCODE_X:
				return (Imagine::Key)MGN_KEY_X;
				break;
			case SDL_SCANCODE_Y:
				return (Imagine::Key)MGN_KEY_Y;
				break;
			case SDL_SCANCODE_Z:
				return (Imagine::Key)MGN_KEY_Z;
				break;
			case SDL_SCANCODE_1:
				return (Imagine::Key)MGN_KEY_1;
				break;
			case SDL_SCANCODE_2:
				return (Imagine::Key)MGN_KEY_2;
				break;
			case SDL_SCANCODE_3:
				return (Imagine::Key)MGN_KEY_3;
				break;
			case SDL_SCANCODE_4:
				return (Imagine::Key)MGN_KEY_4;
				break;
			case SDL_SCANCODE_5:
				return (Imagine::Key)MGN_KEY_5;
				break;
			case SDL_SCANCODE_6:
				return (Imagine::Key)MGN_KEY_6;
				break;
			case SDL_SCANCODE_7:
				return (Imagine::Key)MGN_KEY_7;
				break;
			case SDL_SCANCODE_8:
				return (Imagine::Key)MGN_KEY_8;
				break;
			case SDL_SCANCODE_9:
				return (Imagine::Key)MGN_KEY_9;
				break;
			case SDL_SCANCODE_0:
				return (Imagine::Key)MGN_KEY_0;
				break;
			case SDL_SCANCODE_RETURN:
				return (Imagine::Key)MGN_KEY_ENTER;
				break;
			case SDL_SCANCODE_ESCAPE:
				return (Imagine::Key)MGN_KEY_ESCAPE;
				break;
			case SDL_SCANCODE_BACKSPACE:
				return (Imagine::Key)MGN_KEY_BACKSPACE;
				break;
			case SDL_SCANCODE_TAB:
				return (Imagine::Key)MGN_KEY_TAB;
				break;
			case SDL_SCANCODE_SPACE:
				return (Imagine::Key)MGN_KEY_SPACE;
				break;
			case SDL_SCANCODE_MINUS:
				return (Imagine::Key)MGN_KEY_MINUS;
				break;
			case SDL_SCANCODE_EQUALS:
				return (Imagine::Key)MGN_KEY_EQUAL;
				break;
			case SDL_SCANCODE_LEFTBRACKET:
				return (Imagine::Key)MGN_KEY_LEFT_BRACKET;
				break;
			case SDL_SCANCODE_RIGHTBRACKET:
				return (Imagine::Key)MGN_KEY_RIGHT_BRACKET;
				break;
			case SDL_SCANCODE_NONUSHASH:
			case SDL_SCANCODE_BACKSLASH:
				return (Imagine::Key)MGN_KEY_BACKSLASH;
				break;
			case SDL_SCANCODE_SEMICOLON:
				return (Imagine::Key)MGN_KEY_SEMICOLON;
				break;
			case SDL_SCANCODE_APOSTROPHE:
				return (Imagine::Key)MGN_KEY_APOSTROPHE;
				break;
			case SDL_SCANCODE_GRAVE:
				return (Imagine::Key)MGN_KEY_GRAVE_ACCENT;
				break;
			case SDL_SCANCODE_COMMA:
				return (Imagine::Key)MGN_KEY_COMMA;
				break;
			case SDL_SCANCODE_PERIOD:
				return (Imagine::Key)MGN_KEY_PERIOD;
				break;
			case SDL_SCANCODE_SLASH:
				return (Imagine::Key)MGN_KEY_SLASH;
				break;
			case SDL_SCANCODE_CAPSLOCK:
				return (Imagine::Key)MGN_KEY_CAPS_LOCK;
				break;
			case SDL_SCANCODE_F1:
				return (Imagine::Key)MGN_KEY_F1;
				break;
			case SDL_SCANCODE_F2:
				return (Imagine::Key)MGN_KEY_F2;
				break;
			case SDL_SCANCODE_F3:
				return (Imagine::Key)MGN_KEY_F3;
				break;
			case SDL_SCANCODE_F4:
				return (Imagine::Key)MGN_KEY_F4;
				break;
			case SDL_SCANCODE_F5:
				return (Imagine::Key)MGN_KEY_F5;
				break;
			case SDL_SCANCODE_F6:
				return (Imagine::Key)MGN_KEY_F6;
				break;
			case SDL_SCANCODE_F7:
				return (Imagine::Key)MGN_KEY_F7;
				break;
			case SDL_SCANCODE_F8:
				return (Imagine::Key)MGN_KEY_F8;
				break;
			case SDL_SCANCODE_F9:
				return (Imagine::Key)MGN_KEY_F9;
				break;
			case SDL_SCANCODE_F10:
				return (Imagine::Key)MGN_KEY_F10;
				break;
			case SDL_SCANCODE_F11:
				return (Imagine::Key)MGN_KEY_F11;
				break;
			case SDL_SCANCODE_F12:
				return (Imagine::Key)MGN_KEY_F12;
				break;
			case SDL_SCANCODE_PRINTSCREEN:
				return (Imagine::Key)MGN_KEY_PRINT_SCREEN;
				break;
			case SDL_SCANCODE_SCROLLLOCK:
				return (Imagine::Key)MGN_KEY_SCROLL_LOCK;
				break;
			case SDL_SCANCODE_PAUSE:
				return (Imagine::Key)MGN_KEY_PAUSE;
				break;
			case SDL_SCANCODE_INSERT:
				return (Imagine::Key)MGN_KEY_INSERT;
				break;
			case SDL_SCANCODE_HOME:
				return (Imagine::Key)MGN_KEY_HOME;
				break;
			case SDL_SCANCODE_PAGEUP:
				return (Imagine::Key)MGN_KEY_PAGE_UP;
				break;
			case SDL_SCANCODE_DELETE:
				return (Imagine::Key)MGN_KEY_DELETE;
				break;
			case SDL_SCANCODE_END:
				return (Imagine::Key)MGN_KEY_END;
				break;
			case SDL_SCANCODE_PAGEDOWN:
				return (Imagine::Key)MGN_KEY_PAGE_DOWN;
				break;
			case SDL_SCANCODE_RIGHT:
				return (Imagine::Key)MGN_KEY_RIGHT;
				break;
			case SDL_SCANCODE_LEFT:
				return (Imagine::Key)MGN_KEY_LEFT;
				break;
			case SDL_SCANCODE_DOWN:
				return (Imagine::Key)MGN_KEY_DOWN;
				break;
			case SDL_SCANCODE_UP:
				return (Imagine::Key)MGN_KEY_UP;
				break;
			case SDL_SCANCODE_KP_DIVIDE:
				return (Imagine::Key)MGN_KEY_KP_DIVIDE;
				break;
			case SDL_SCANCODE_KP_MULTIPLY:
				return (Imagine::Key)MGN_KEY_KP_MULTIPLY;
				break;
			case SDL_SCANCODE_KP_MINUS:
				return (Imagine::Key)MGN_KEY_KP_SUBTRACT;
				break;
			case SDL_SCANCODE_KP_PLUS:
				return (Imagine::Key)MGN_KEY_KP_ADD;
				break;
			case SDL_SCANCODE_KP_ENTER:
				return (Imagine::Key)MGN_KEY_KP_ENTER;
				break;
			case SDL_SCANCODE_KP_1:
				return (Imagine::Key)MGN_KEY_KP_1;
				break;
			case SDL_SCANCODE_KP_2:
				return (Imagine::Key)MGN_KEY_KP_2;
				break;
			case SDL_SCANCODE_KP_3:
				return (Imagine::Key)MGN_KEY_KP_3;
				break;
			case SDL_SCANCODE_KP_4:
				return (Imagine::Key)MGN_KEY_KP_4;
				break;
			case SDL_SCANCODE_KP_5:
				return (Imagine::Key)MGN_KEY_KP_5;
				break;
			case SDL_SCANCODE_KP_6:
				return (Imagine::Key)MGN_KEY_KP_6;
				break;
			case SDL_SCANCODE_KP_7:
				return (Imagine::Key)MGN_KEY_KP_7;
				break;
			case SDL_SCANCODE_KP_8:
				return (Imagine::Key)MGN_KEY_KP_8;
				break;
			case SDL_SCANCODE_KP_9:
				return (Imagine::Key)MGN_KEY_KP_9;
				break;
			case SDL_SCANCODE_KP_0:
				return (Imagine::Key)MGN_KEY_KP_0;
				break;
			case SDL_SCANCODE_KP_PERIOD:
				return (Imagine::Key)MGN_KEY_KP_DECIMAL;
				break;
			case SDL_SCANCODE_NONUSBACKSLASH:
				return (Imagine::Key)MGN_KEY_WORLD_1;
				break;
			case SDL_SCANCODE_KP_EQUALS:
				return (Imagine::Key)MGN_KEY_KP_EQUAL;
				break;
			case SDL_SCANCODE_F13:
				return (Imagine::Key)MGN_KEY_F13;
				break;
			case SDL_SCANCODE_F14:
				return (Imagine::Key)MGN_KEY_F14;
				break;
			case SDL_SCANCODE_F15:
				return (Imagine::Key)MGN_KEY_F15;
				break;
			case SDL_SCANCODE_F16:
				return (Imagine::Key)MGN_KEY_F16;
				break;
			case SDL_SCANCODE_F17:
				return (Imagine::Key)MGN_KEY_F17;
				break;
			case SDL_SCANCODE_F18:
				return (Imagine::Key)MGN_KEY_F18;
				break;
			case SDL_SCANCODE_F19:
				return (Imagine::Key)MGN_KEY_F19;
				break;
			case SDL_SCANCODE_F20:
				return (Imagine::Key)MGN_KEY_F20;
				break;
			case SDL_SCANCODE_F21:
				return (Imagine::Key)MGN_KEY_F21;
				break;
			case SDL_SCANCODE_F22:
				return (Imagine::Key)MGN_KEY_F22;
				break;
			case SDL_SCANCODE_F23:
				return (Imagine::Key)MGN_KEY_F23;
				break;
			case SDL_SCANCODE_F24:
				return (Imagine::Key)MGN_KEY_F24;
				break;
			case SDL_SCANCODE_MENU:
				return (Imagine::Key)MGN_KEY_MENU;
				break;
			case SDL_SCANCODE_KP_EQUALSAS400:
				return (Imagine::Key)MGN_KEY_KP_EQUAL;
				break;
			case SDL_SCANCODE_KP_DECIMAL:
				return (Imagine::Key)MGN_KEY_KP_DECIMAL;
				break;
			case SDL_SCANCODE_LCTRL:
				return (Imagine::Key)MGN_KEY_LEFT_CONTROL;
				break;
			case SDL_SCANCODE_LSHIFT:
				return (Imagine::Key)MGN_KEY_LEFT_SHIFT;
				break;
			case SDL_SCANCODE_LALT:
				return (Imagine::Key)MGN_KEY_LEFT_ALT;
				break;
			case SDL_SCANCODE_LGUI:
				return (Imagine::Key)MGN_KEY_LEFT_SUPER;
				break;
			case SDL_SCANCODE_RCTRL:
				return (Imagine::Key)MGN_KEY_RIGHT_CONTROL;
				break;
			case SDL_SCANCODE_RSHIFT:
				return (Imagine::Key)MGN_KEY_RIGHT_SHIFT;
				break;
			case SDL_SCANCODE_RALT:
				return (Imagine::Key)MGN_KEY_RIGHT_ALT;
				break;
			case SDL_SCANCODE_RGUI:
				return (Imagine::Key)MGN_KEY_RIGHT_SUPER;
				break;
			default:
				return Imagine::Key::Unknown;
				break;
		}
	}

} // SDL3
} // Imagine
