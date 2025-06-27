cmake_minimum_required(VERSION 3.25)

target_link_libraries(Core PUBLIC SDL3::SDL3)

target_include_directories(Core PRIVATE Sources)
target_include_directories(Core PRIVATE Includes)

set(SDL3_WINDOW_SOURCES
		Includes/Imagine/SDL3/SDL3Window.hpp
	Sources/SDL3Window.cpp
	Sources/SDL3Inputs.cpp
		Includes/Imagine/SDL3/SDL3Inputs.hpp
)

target_sources(Core PRIVATE
		${SDL3_WINDOW_SOURCES}
)

if(MGN_IMGUI)
	target_sources(Core PRIVATE
			Sources/SDL3ImGui.cpp
	)
endif()

target_compile_definitions(Core PUBLIC MGN_WINDOW_SDL3=1)
