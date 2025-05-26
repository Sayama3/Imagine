cmake_minimum_required(VERSION 3.25)

find_package(Vulkan REQUIRED)
target_link_libraries(Core PUBLIC
		Vulkan::Vulkan
		vk-bootstrap::vk-bootstrap
		GPUOpen::VulkanMemoryAllocator
		volk::volk
)

target_include_directories(Core PRIVATE Sources)
target_include_directories(Core PRIVATE Include)

set(VULKAN_SOURCES
		Include/Imagine/Vulkan/VulkanRenderer.hpp
		Sources/VulkanRenderer.cpp
		Include/Imagine/Vulkan/VulkanInitializer.hpp
		Include/Imagine/Vulkan/VulkanUtils.hpp
		Include/Imagine/Vulkan/VulkanMacros.hpp
)

target_sources(Core PRIVATE
	${VULKAN_SOURCES}
)

target_precompile_headers(Core PRIVATE
	<vulkan/vulkan.h>
	<vulkan/vulkan.hpp>
)

target_compile_definitions(Core PUBLIC MGN_RENDERER_VULKAN=1)
