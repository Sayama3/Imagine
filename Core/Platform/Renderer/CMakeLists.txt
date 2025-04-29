cmake_minimum_required(VERSION 3.25)

if (MGN_RENDERER_VULKAN)
	add_subdirectory(VULKAN)
else ()
	add_subdirectory(CPU_RENDERER)
endif ()