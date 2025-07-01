//
// Created by Iannis on 23/06/2025.
//

#pragma once
#include <SDL3/SDL.h>
#include "Imagine/Core/Inputs.hpp"



namespace Imagine::SDL3 {

	inline Imagine::Mouse GetMouseButton(const SDL_MouseButtonFlags mb) {
		switch (mb) {
			case SDL_BUTTON_LEFT: return Imagine::Mouse::Left;
			case SDL_BUTTON_MIDDLE: return Imagine::Mouse::Middle;
			case SDL_BUTTON_RIGHT: return Imagine::Mouse::Right;
			case SDL_BUTTON_X1: return Imagine::Mouse::Button4;
			case SDL_BUTTON_X2: return Imagine::Mouse::Button5;
			default: return static_cast<Imagine::Mouse>(mb - 1);
		}
	}

	inline Imagine::Key GetKeyCode(const SDL_Scancode sc) {
		switch (sc) {
			case SDL_SCANCODE_A: return Imagine::Key::A;
			case SDL_SCANCODE_B: return Imagine::Key::B;
			case SDL_SCANCODE_C: return Imagine::Key::C;
			case SDL_SCANCODE_D: return Imagine::Key::D;
			case SDL_SCANCODE_E: return Imagine::Key::E;
			case SDL_SCANCODE_F: return Imagine::Key::F;
			case SDL_SCANCODE_G: return Imagine::Key::G;
			case SDL_SCANCODE_H: return Imagine::Key::H;
			case SDL_SCANCODE_I: return Imagine::Key::I;
			case SDL_SCANCODE_J: return Imagine::Key::J;
			case SDL_SCANCODE_K: return Imagine::Key::K;
			case SDL_SCANCODE_L: return Imagine::Key::L;
			case SDL_SCANCODE_M: return Imagine::Key::M;
			case SDL_SCANCODE_N: return Imagine::Key::N;
			case SDL_SCANCODE_O: return Imagine::Key::O;
			case SDL_SCANCODE_P: return Imagine::Key::P;
			case SDL_SCANCODE_Q: return Imagine::Key::Q;
			case SDL_SCANCODE_R: return Imagine::Key::R;
			case SDL_SCANCODE_S: return Imagine::Key::S;
			case SDL_SCANCODE_T: return Imagine::Key::T;
			case SDL_SCANCODE_U: return Imagine::Key::U;
			case SDL_SCANCODE_V: return Imagine::Key::V;
			case SDL_SCANCODE_W: return Imagine::Key::W;
			case SDL_SCANCODE_X: return Imagine::Key::X;
			case SDL_SCANCODE_Y: return Imagine::Key::Y;
			case SDL_SCANCODE_Z: return Imagine::Key::Z;
			case SDL_SCANCODE_1: return Imagine::Key::Key1;
			case SDL_SCANCODE_2: return Imagine::Key::Key2;
			case SDL_SCANCODE_3: return Imagine::Key::Key3;
			case SDL_SCANCODE_4: return Imagine::Key::Key4;
			case SDL_SCANCODE_5: return Imagine::Key::Key5;
			case SDL_SCANCODE_6: return Imagine::Key::Key6;
			case SDL_SCANCODE_7: return Imagine::Key::Key7;
			case SDL_SCANCODE_8: return Imagine::Key::Key8;
			case SDL_SCANCODE_9: return Imagine::Key::Key9;
			case SDL_SCANCODE_0: return Imagine::Key::Key0;
			case SDL_SCANCODE_RETURN: return Imagine::Key::Enter;
			case SDL_SCANCODE_ESCAPE: return Imagine::Key::Escape;
			case SDL_SCANCODE_BACKSPACE: return Imagine::Key::Backspace;
			case SDL_SCANCODE_TAB: return Imagine::Key::Tab;
			case SDL_SCANCODE_SPACE: return Imagine::Key::Space;
			case SDL_SCANCODE_MINUS: return Imagine::Key::Minus;
			case SDL_SCANCODE_EQUALS: return Imagine::Key::Equal;
			case SDL_SCANCODE_LEFTBRACKET: return Imagine::Key::LeftBracket;
			case SDL_SCANCODE_RIGHTBRACKET: return Imagine::Key::RightBracket;
			case SDL_SCANCODE_NONUSHASH:
			case SDL_SCANCODE_BACKSLASH: return Imagine::Key::Backslash;
			case SDL_SCANCODE_SEMICOLON: return Imagine::Key::Semicolon;
			case SDL_SCANCODE_APOSTROPHE: return Imagine::Key::Apostrophe;
			case SDL_SCANCODE_GRAVE: return Imagine::Key::GraveAccent;
			case SDL_SCANCODE_COMMA: return Imagine::Key::Comma;
			case SDL_SCANCODE_PERIOD: return Imagine::Key::Period;
			case SDL_SCANCODE_SLASH: return Imagine::Key::Slash;
			case SDL_SCANCODE_CAPSLOCK: return Imagine::Key::CapsLock;
			case SDL_SCANCODE_F1: return Imagine::Key::F1;
			case SDL_SCANCODE_F2: return Imagine::Key::F2;
			case SDL_SCANCODE_F3: return Imagine::Key::F3;
			case SDL_SCANCODE_F4: return Imagine::Key::F4;
			case SDL_SCANCODE_F5: return Imagine::Key::F5;
			case SDL_SCANCODE_F6: return Imagine::Key::F6;
			case SDL_SCANCODE_F7: return Imagine::Key::F7;
			case SDL_SCANCODE_F8: return Imagine::Key::F8;
			case SDL_SCANCODE_F9: return Imagine::Key::F9;
			case SDL_SCANCODE_F10: return Imagine::Key::F10;
			case SDL_SCANCODE_F11: return Imagine::Key::F11;
			case SDL_SCANCODE_F12: return Imagine::Key::F12;
			case SDL_SCANCODE_PRINTSCREEN: return Imagine::Key::PrintScreen;
			case SDL_SCANCODE_SCROLLLOCK: return Imagine::Key::ScrollLock;
			case SDL_SCANCODE_PAUSE: return Imagine::Key::Pause;
			case SDL_SCANCODE_INSERT: return Imagine::Key::Insert;
			case SDL_SCANCODE_HOME: return Imagine::Key::Home;
			case SDL_SCANCODE_PAGEUP: return Imagine::Key::PageUp;
			case SDL_SCANCODE_DELETE: return Imagine::Key::Delete;
			case SDL_SCANCODE_END: return Imagine::Key::End;
			case SDL_SCANCODE_PAGEDOWN: return Imagine::Key::PageDown;
			case SDL_SCANCODE_RIGHT: return Imagine::Key::Right;
			case SDL_SCANCODE_LEFT: return Imagine::Key::Left;
			case SDL_SCANCODE_DOWN: return Imagine::Key::Down;
			case SDL_SCANCODE_UP: return Imagine::Key::Up;
			case SDL_SCANCODE_KP_DIVIDE: return Imagine::Key::KpDivide;
			case SDL_SCANCODE_KP_MULTIPLY: return Imagine::Key::KpMultiply;
			case SDL_SCANCODE_KP_MINUS: return Imagine::Key::KpSubtract;
			case SDL_SCANCODE_KP_PLUS: return Imagine::Key::KpAdd;
			case SDL_SCANCODE_KP_ENTER: return Imagine::Key::KpEnter;
			case SDL_SCANCODE_KP_1: return Imagine::Key::Kp1;
			case SDL_SCANCODE_KP_2: return Imagine::Key::Kp2;
			case SDL_SCANCODE_KP_3: return Imagine::Key::Kp3;
			case SDL_SCANCODE_KP_4: return Imagine::Key::Kp4;
			case SDL_SCANCODE_KP_5: return Imagine::Key::Kp5;
			case SDL_SCANCODE_KP_6: return Imagine::Key::Kp6;
			case SDL_SCANCODE_KP_7: return Imagine::Key::Kp7;
			case SDL_SCANCODE_KP_8: return Imagine::Key::Kp8;
			case SDL_SCANCODE_KP_9: return Imagine::Key::Kp9;
			case SDL_SCANCODE_KP_0: return Imagine::Key::Kp0;
			case SDL_SCANCODE_KP_PERIOD: return Imagine::Key::KpDecimal;
			case SDL_SCANCODE_NONUSBACKSLASH: return Imagine::Key::World1;
			case SDL_SCANCODE_KP_EQUALS: return Imagine::Key::KpEqual;
			case SDL_SCANCODE_F13: return Imagine::Key::F13;
			case SDL_SCANCODE_F14: return Imagine::Key::F14;
			case SDL_SCANCODE_F15: return Imagine::Key::F15;
			case SDL_SCANCODE_F16: return Imagine::Key::F16;
			case SDL_SCANCODE_F17: return Imagine::Key::F17;
			case SDL_SCANCODE_F18: return Imagine::Key::F18;
			case SDL_SCANCODE_F19: return Imagine::Key::F19;
			case SDL_SCANCODE_F20: return Imagine::Key::F20;
			case SDL_SCANCODE_F21: return Imagine::Key::F21;
			case SDL_SCANCODE_F22: return Imagine::Key::F22;
			case SDL_SCANCODE_F23: return Imagine::Key::F23;
			case SDL_SCANCODE_F24: return Imagine::Key::F24;
			case SDL_SCANCODE_MENU: return Imagine::Key::Menu;
			case SDL_SCANCODE_KP_EQUALSAS400: return Imagine::Key::KpEqual;
			case SDL_SCANCODE_KP_DECIMAL: return Imagine::Key::KpDecimal;
			case SDL_SCANCODE_LCTRL: return Imagine::Key::LeftControl;
			case SDL_SCANCODE_LSHIFT: return Imagine::Key::LeftShift;
			case SDL_SCANCODE_LALT: return Imagine::Key::LeftAlt;
			case SDL_SCANCODE_LGUI: return Imagine::Key::LeftSuper;
			case SDL_SCANCODE_RCTRL: return Imagine::Key::RightControl;
			case SDL_SCANCODE_RSHIFT: return Imagine::Key::RightShift;
			case SDL_SCANCODE_RALT: return Imagine::Key::RightAlt;
			case SDL_SCANCODE_RGUI: return Imagine::Key::RightSuper;
			default: return Imagine::Key::Unknown;
		}
	}

} // SDL3
// Imagine
