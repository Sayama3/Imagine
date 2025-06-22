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
		Sources/VulkanFrameData.cpp
		Include/Imagine/Vulkan/Vulkan.hpp
		Sources/Vulkan.cpp
		Include/Imagine/Vulkan/VulkanDeleter.hpp
		Sources/VulkanDeleter.cpp
		Include/Imagine/Vulkan/VulkanImage.hpp
		Sources/VulkanImage.cpp
		Include/Imagine/Vulkan/VulkanShader.hpp
		Sources/VulkanShader.cpp
		Include/Imagine/Vulkan/PipelineBuilder.hpp
		Sources/PipelineBuilder.cpp
		Sources/VulkanInitializer.cpp
		Include/Imagine/Vulkan/Descriptors.hpp
		Sources/Descriptors.cpp
		Include/Imagine/Vulkan/VulkanRenderObject.hpp
		Sources/VulkanRenderObject.cpp
		Include/Imagine/Vulkan/VulkanMaterial.hpp
		Sources/VulkanMaterial.cpp
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
