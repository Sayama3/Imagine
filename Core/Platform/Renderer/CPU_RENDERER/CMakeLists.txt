cmake_minimum_required(VERSION 3.25)

target_include_directories(Core PRIVATE Sources)

set(CPU_RENDERER_SOURCES
	Sources/CPURenderer.hpp
	Sources/CPURenderer.cpp
)

target_sources(Core PRIVATE
	${CPU_RENDERER_SOURCES}
)
target_compile_definitions(Core PUBLIC MGN_RENDERER_CPU=1)