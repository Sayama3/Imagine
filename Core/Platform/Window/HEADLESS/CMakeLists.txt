cmake_minimum_required(VERSION 3.25)

target_include_directories(Core PRIVATE Sources)

set(HEADLESS_WINDOW_SOURCES
		Sources/HeadLessWindow.hpp
		Sources/HeadLessWindow.cpp
)

target_sources(Core PRIVATE
		${HEADLESS_WINDOW_SOURCES}
)

target_compile_definitions(Core PUBLIC MGN_WINDOW_HEADLESS=1)