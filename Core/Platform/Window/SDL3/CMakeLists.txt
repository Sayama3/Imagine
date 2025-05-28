cmake_minimum_required(VERSION 3.25)

target_link_libraries(Core PUBLIC SDL3::SDL3)

target_include_directories(Core PRIVATE Sources)

set(SDL3_WINDOW_SOURCES
	Sources/SDL3Window.hpp
	Sources/SDL3Window.cpp
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
