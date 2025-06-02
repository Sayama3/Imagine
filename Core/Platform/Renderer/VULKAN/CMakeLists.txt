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
		Include/Imagine/Vulkan/VulkanTypes.hpp
		Include/Imagine/Vulkan/VulkanRenderer.hpp
		Sources/VulkanRenderer.cpp
		Include/Imagine/Vulkan/VulkanInitializer.hpp
		Include/Imagine/Vulkan/VulkanUtils.hpp
		Include/Imagine/Vulkan/VulkanMacros.hpp
		Include/Imagine/Vulkan/VulkanFrameData.hpp
		Include/Imagine/Vulkan/Vulkan.hpp
		Sources/Vulkan.cpp
		Include/Imagine/Vulkan/VulkanDeleter.hpp
		Include/Imagine/Vulkan/AllocatedImage.hpp
		Sources/AllocatedImage.cpp
		Include/Imagine/Vulkan/DescriptorLayoutBuilder.hpp
		Sources/DescriptorLayoutBuilder.cpp
		Include/Imagine/Vulkan/DescriptorAllocator.hpp
		Sources/DescriptorAllocator.cpp
		Include/Imagine/Vulkan/VulkanShader.hpp
		Sources/VulkanShader.cpp
		Include/Imagine/Vulkan/PipelineBuilder.hpp
		Sources/PipelineBuilder.cpp
)

target_sources(Core PRIVATE
	${VULKAN_SOURCES}
)

if(MGN_IMGUI)
	target_sources(Core PRIVATE
		Sources/VulkanImGui.cpp
	)
endif()

target_compile_definitions(Core PUBLIC MGN_RENDERER_VULKAN=1)
