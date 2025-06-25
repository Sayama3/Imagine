//
// Created by ianpo on 19/07/2023.
//

#pragma once

// Imported from GLFW3.h

/* The unknown key */
#define MGN_KEY_UNKNOWN            -1

/* Printable keys */
#define MGN_KEY_SPACE              32
#define MGN_KEY_APOSTROPHE         39  /* ' */
#define MGN_KEY_COMMA              44  /* , */
#define MGN_KEY_MINUS              45  /* - */
#define MGN_KEY_PERIOD             46  /* . */
#define MGN_KEY_SLASH              47  /* / */
#define MGN_KEY_0                  48
#define MGN_KEY_1                  49
#define MGN_KEY_2                  50
#define MGN_KEY_3                  51
#define MGN_KEY_4                  52
#define MGN_KEY_5                  53
#define MGN_KEY_6                  54
#define MGN_KEY_7                  55
#define MGN_KEY_8                  56
#define MGN_KEY_9                  57
#define MGN_KEY_SEMICOLON          59  /* ; */
#define MGN_KEY_EQUAL              61  /* = */
#define MGN_KEY_A                  65
#define MGN_KEY_B                  66
#define MGN_KEY_C                  67
#define MGN_KEY_D                  68
#define MGN_KEY_E                  69
#define MGN_KEY_F                  70
#define MGN_KEY_G                  71
#define MGN_KEY_H                  72
#define MGN_KEY_I                  73
#define MGN_KEY_J                  74
#define MGN_KEY_K                  75
#define MGN_KEY_L                  76
#define MGN_KEY_M                  77
#define MGN_KEY_N                  78
#define MGN_KEY_O                  79
#define MGN_KEY_P                  80
#define MGN_KEY_Q                  81
#define MGN_KEY_R                  82
#define MGN_KEY_S                  83
#define MGN_KEY_T                  84
#define MGN_KEY_U                  85
#define MGN_KEY_V                  86
#define MGN_KEY_W                  87
#define MGN_KEY_X                  88
#define MGN_KEY_Y                  89
#define MGN_KEY_Z                  90
#define MGN_KEY_LEFT_BRACKET       91  /* [ */
#define MGN_KEY_BACKSLASH          92  /* \ */
#define MGN_KEY_RIGHT_BRACKET      93  /* ] */
#define MGN_KEY_GRAVE_ACCENT       96  /* ` */
#define MGN_KEY_WORLD_1            161 /* non-US #1 */
#define MGN_KEY_WORLD_2            162 /* non-US #2 */


/* Function keys */
#define MGN_KEY_ESCAPE             256
#define MGN_KEY_ENTER              257
#define MGN_KEY_TAB                258
#define MGN_KEY_BACKSPACE          259
#define MGN_KEY_INSERT             260
#define MGN_KEY_DELETE             261
#define MGN_KEY_RIGHT              262
#define MGN_KEY_LEFT               263
#define MGN_KEY_DOWN               264
#define MGN_KEY_UP                 265
#define MGN_KEY_PAGE_UP            266
#define MGN_KEY_PAGE_DOWN          267
#define MGN_KEY_HOME               268
#define MGN_KEY_END                269
#define MGN_KEY_CAPS_LOCK          280
#define MGN_KEY_SCROLL_LOCK        281
#define MGN_KEY_NUM_LOCK           282
#define MGN_KEY_PRINT_SCREEN       283
#define MGN_KEY_PAUSE              284
#define MGN_KEY_F1                 290
#define MGN_KEY_F2                 291
#define MGN_KEY_F3                 292
#define MGN_KEY_F4                 293
#define MGN_KEY_F5                 294
#define MGN_KEY_F6                 295
#define MGN_KEY_F7                 296
#define MGN_KEY_F8                 297
#define MGN_KEY_F9                 298
#define MGN_KEY_F10                299
#define MGN_KEY_F11                300
#define MGN_KEY_F12                301
#define MGN_KEY_F13                302
#define MGN_KEY_F14                303
#define MGN_KEY_F15                304
#define MGN_KEY_F16                305
#define MGN_KEY_F17                306
#define MGN_KEY_F18                307
#define MGN_KEY_F19                308
#define MGN_KEY_F20                309
#define MGN_KEY_F21                310
#define MGN_KEY_F22                311
#define MGN_KEY_F23                312
#define MGN_KEY_F24                313
#define MGN_KEY_F25                314
#define MGN_KEY_KP_0               320
#define MGN_KEY_KP_1               321
#define MGN_KEY_KP_2               322
#define MGN_KEY_KP_3               323
#define MGN_KEY_KP_4               324
#define MGN_KEY_KP_5               325
#define MGN_KEY_KP_6               326
#define MGN_KEY_KP_7               327
#define MGN_KEY_KP_8               328
#define MGN_KEY_KP_9               329
#define MGN_KEY_KP_DECIMAL         330
#define MGN_KEY_KP_DIVIDE          331
#define MGN_KEY_KP_MULTIPLY        332
#define MGN_KEY_KP_SUBTRACT        333
#define MGN_KEY_KP_ADD             334
#define MGN_KEY_KP_ENTER           335
#define MGN_KEY_KP_EQUAL           336
#define MGN_KEY_LEFT_SHIFT         340
#define MGN_KEY_LEFT_CONTROL       341
#define MGN_KEY_LEFT_ALT           342
#define MGN_KEY_LEFT_SUPER         343
#define MGN_KEY_RIGHT_SHIFT        344
#define MGN_KEY_RIGHT_CONTROL      345
#define MGN_KEY_RIGHT_ALT          346
#define MGN_KEY_RIGHT_SUPER        347
#define MGN_KEY_MENU               348
#define MGN_KEY_LAST               MGN_KEY_MENU

namespace Imagine {

	enum class Key : int32_t {
		Unknown = MGN_KEY_UNKNOWN,
		Space = MGN_KEY_SPACE,
		Apostrophe = MGN_KEY_APOSTROPHE,
		Comma = MGN_KEY_COMMA,
		Minus = MGN_KEY_MINUS,
		Period = MGN_KEY_PERIOD,
		Slash = MGN_KEY_SLASH,
		Key0 = MGN_KEY_0,
		Key1 = MGN_KEY_1,
		Key2 = MGN_KEY_2,
		Key3 = MGN_KEY_3,
		Key4 = MGN_KEY_4,
		Key5 = MGN_KEY_5,
		Key6 = MGN_KEY_6,
		Key7 = MGN_KEY_7,
		Key8 = MGN_KEY_8,
		Key9 = MGN_KEY_9,
		Semicolon = MGN_KEY_SEMICOLON,
		Equal = MGN_KEY_EQUAL,
		A = MGN_KEY_A,
		B = MGN_KEY_B,
		C = MGN_KEY_C,
		D = MGN_KEY_D,
		E = MGN_KEY_E,
		F = MGN_KEY_F,
		G = MGN_KEY_G,
		H = MGN_KEY_H,
		I = MGN_KEY_I,
		J = MGN_KEY_J,
		K = MGN_KEY_K,
		L = MGN_KEY_L,
		M = MGN_KEY_M,
		N = MGN_KEY_N,
		O = MGN_KEY_O,
		P = MGN_KEY_P,
		Q = MGN_KEY_Q,
		R = MGN_KEY_R,
		S = MGN_KEY_S,
		T = MGN_KEY_T,
		U = MGN_KEY_U,
		V = MGN_KEY_V,
		W = MGN_KEY_W,
		X = MGN_KEY_X,
		Y = MGN_KEY_Y,
		Z = MGN_KEY_Z,
		LeftBracket = MGN_KEY_LEFT_BRACKET,
		Backslash = MGN_KEY_BACKSLASH,
		RightBracket = MGN_KEY_RIGHT_BRACKET,
		GraveAccent = MGN_KEY_GRAVE_ACCENT,
		World1 = MGN_KEY_WORLD_1,
		World2 = MGN_KEY_WORLD_2,

		Escape = MGN_KEY_ESCAPE,
		Enter = MGN_KEY_ENTER,
		Tab = MGN_KEY_TAB,
		Backspace = MGN_KEY_BACKSPACE,
		Insert = MGN_KEY_INSERT,
		Delete = MGN_KEY_DELETE,
		Right = MGN_KEY_RIGHT,
		Left = MGN_KEY_LEFT,
		Down = MGN_KEY_DOWN,
		Up = MGN_KEY_UP,
		PageUp = MGN_KEY_PAGE_UP,
		PageDown = MGN_KEY_PAGE_DOWN,
		Home = MGN_KEY_HOME,
		End = MGN_KEY_END,
		CapsLock = MGN_KEY_CAPS_LOCK,
		ScrollLock = MGN_KEY_SCROLL_LOCK,
		NumLock = MGN_KEY_NUM_LOCK,
		PrintScreen = MGN_KEY_PRINT_SCREEN,
		Pause = MGN_KEY_PAUSE,
		F1 = MGN_KEY_F1,
		F2 = MGN_KEY_F2,
		F3 = MGN_KEY_F3,
		F4 = MGN_KEY_F4,
		F5 = MGN_KEY_F5,
		F6 = MGN_KEY_F6,
		F7 = MGN_KEY_F7,
		F8 = MGN_KEY_F8,
		F9 = MGN_KEY_F9,
		F10 = MGN_KEY_F10,
		F11 = MGN_KEY_F11,
		F12 = MGN_KEY_F12,
		F13 = MGN_KEY_F13,
		F14 = MGN_KEY_F14,
		F15 = MGN_KEY_F15,
		F16 = MGN_KEY_F16,
		F17 = MGN_KEY_F17,
		F18 = MGN_KEY_F18,
		F19 = MGN_KEY_F19,
		F20 = MGN_KEY_F20,
		F21 = MGN_KEY_F21,
		F22 = MGN_KEY_F22,
		F23 = MGN_KEY_F23,
		F24 = MGN_KEY_F24,
		F25 = MGN_KEY_F25,
		Kp0 = MGN_KEY_KP_0,
		Kp1 = MGN_KEY_KP_1,
		Kp2 = MGN_KEY_KP_2,
		Kp3 = MGN_KEY_KP_3,
		Kp4 = MGN_KEY_KP_4,
		Kp5 = MGN_KEY_KP_5,
		Kp6 = MGN_KEY_KP_6,
		Kp7 = MGN_KEY_KP_7,
		Kp8 = MGN_KEY_KP_8,
		Kp9 = MGN_KEY_KP_9,
		KpDecimal = MGN_KEY_KP_DECIMAL,
		KpDivide = MGN_KEY_KP_DIVIDE,
		KpMultiply = MGN_KEY_KP_MULTIPLY,
		KpSubtract = MGN_KEY_KP_SUBTRACT,
		KpAdd = MGN_KEY_KP_ADD,
		KpEnter = MGN_KEY_KP_ENTER,
		KpEqual = MGN_KEY_KP_EQUAL,
		LeftShift = MGN_KEY_LEFT_SHIFT,
		LeftControl = MGN_KEY_LEFT_CONTROL,
		LeftAlt = MGN_KEY_LEFT_ALT,
		LeftSuper = MGN_KEY_LEFT_SUPER,
		RightShift = MGN_KEY_RIGHT_SHIFT,
		RightControl = MGN_KEY_RIGHT_CONTROL,
		RightAlt = MGN_KEY_RIGHT_ALT,
		RightSuper = MGN_KEY_RIGHT_SUPER,
		Menu = MGN_KEY_MENU,
		Last = MGN_KEY_LAST,
	};
}