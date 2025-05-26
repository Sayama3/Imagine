cmake_minimum_required(VERSION 3.25)

if (MGN_WINDOW_GLFW)
	add_subdirectory(GLFW)
elseif (MGN_WINDOW_SDL3)
	add_subdirectory(SDL3)
else ()
	add_subdirectory(HEADLESS)
endif ()