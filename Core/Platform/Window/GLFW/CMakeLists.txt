cmake_minimum_required(VERSION 3.25)

target_link_libraries(Core PUBLIC glfw)

target_include_directories(Core PRIVATE Sources)

set(GLFW_WINDOW_SOURCES
        Sources/GLFWWindow.hpp
        Sources/GLFWWindow.cpp
)

target_sources(Core PRIVATE
		${GLFW_WINDOW_SOURCES}
)

if(MGN_IMGUI)
	target_sources(Core PRIVATE
			Sources/GLFWImGui.cpp
	)
endif()

target_compile_definitions(Core PUBLIC MGN_WINDOW_GLFW=1)
