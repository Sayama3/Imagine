cmake_minimum_required(VERSION 3.25)

find_package(Vulkan REQUIRED)
target_link_libraries(Core PUBLIC Vulkan::Vulkan)

target_include_directories(Core PRIVATE Sources)

set(VULKAN_SOURCES
		Sources/Imagine/Vulkan/VulkanRenderer.hpp
		Sources/Imagine/Vulkan/VulkanRenderer.cpp
)

target_sources(Core PRIVATE
	${VULKAN_SOURCES}
)

target_precompile_headers(Core PRIVATE
	<vulkan/vulkan.h>
	<vulkan/vulkan.hpp>
)
target_compile_definitions(Core PUBLIC MGN_RENDERER_VULKAN=1)
