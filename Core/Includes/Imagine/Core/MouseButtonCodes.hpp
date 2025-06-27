//
// Created by ianpo on 19/07/2023.
//

#pragma once

// Imported from glfw3.h

namespace Imagine {
	enum class Mouse : int32_t
	{
		Button1 = 0,
		Button2 = 1,
		Button3 = 2,
		Button4 = 3,
		Button5 = 4,
		Button6 = 5,
		Button7 = 6,
		Button8 = 7,
		Last = Button8,
		Left = Button1,
		Right = Button2,
		Middle = Button3,
	};
}