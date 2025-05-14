cmake_minimum_required(VERSION 3.25)

set(APPLICATION_SOURCES
	Sources/main.cpp
		Includes/Imagine/Header.hpp
)

add_executable(Imagine
		${APPLICATION_SOURCES}
)

target_link_libraries(Imagine PUBLIC
		Imagine::Core
)

target_precompile_headers(Imagine REUSE_FROM Core)

target_include_directories(Imagine PUBLIC Includes)
target_include_directories(Imagine PRIVATE Sources)

# Might have to do something with that
#message(FATAL_ERROR "$ENV{VULKAN_SDK}/Bin/glslc")

if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
	file(CREATE_LINK ${CMAKE_SOURCE_DIR}/Assets/ ${CMAKE_CURRENT_BINARY_DIR}/Assets/ RESULT copy_result COPY_ON_ERROR SYMBOLIC)
	if(NOT (copy_result EQUAL 0))
		message(WARNING
				"Copy made instead of symlink.\n"
				"${copy_result}"
		)
	endif()
else()
	file(COPY ${CMAKE_SOURCE_DIR}/Assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Assets/)
endif()