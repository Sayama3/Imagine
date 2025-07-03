cmake_minimum_required(VERSION 3.16)

# Use sub-modules instead of FetchContent because I can work offline this way.

#include(FetchContent)

set(GLM_ENABLE_CXX_20 ON)
set(GLM_BUILD_LIBRARY OFF)
set(GLM_ENABLE_FAST_MATH ON)
add_subdirectory(glm)

add_subdirectory(spdlog)

set(TRACY_ENABLE ON)
set(TRACY_STATIC ON)
set(TRACY_ON_DEMAND ON)
add_subdirectory(tracy)

set(XXHASH_BUILD_XXHSUM OFF)
add_subdirectory(xxHash/build/cmake)
#FetchContent_Declare(xxHash
#		GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
#		GIT_TAG "1fc167c19f009265eb0a37117a03bbcdbc687e59" # Commit of the 2 June 2025
#		SOURCE_SUBDIR  build/cmake
#)

#FetchContent_Declare(
#		EASTL
#		GIT_REPOSITORY "https://github.com/electronicarts/EASTL.git"
#		GIT_TAG "3.21.23"
#		GIT_PROGRESS TRUE
#)

set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
add_subdirectory(assimp)


set(MESHOPT_BUILD_DEMO OFF) # Build demo
set(MESHOPT_BUILD_GLTFPACK OFF) # Build gltfpack
set(MESHOPT_BUILD_SHARED_LIBS OFF) # Build shared libraries
set(MESHOPT_WERROR OFF) # Treat warnings as errors
set(MESHOPT_INSTALL OFF) # Install library
add_subdirectory(meshoptimizer)

add_subdirectory(bitsery)

# ===== Window Libraries =====

if(MGN_WINDOW_GLFW)
	set(IMGUI_GLFW ON)
	add_subdirectory(glfw)
elseif(MGN_WINDOW_SDL3)
	set(IMGUI_SDL3 ON)
	set(SDL_STATIC ON)
	set(SDL_SHARED OFF)
	add_subdirectory(SDL)
endif()

# ===== Renderer Libraries =====

if(MGN_RENDERER_VULKAN)
	set(SDL_VULKAN ON)
	set(SDL_RENDER_VULKAN ON)
	set(IMGUI_VULKAN ON)

	add_subdirectory(vk-bootstrap)

	add_subdirectory(VulkanMemoryAllocator)
	if (WIN32)
		set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
	#TODO? Add other VOLK_STATIC_DEFINES
	endif()
	add_subdirectory(volk)
endif()

# ===== IMGUI =====

if(MGN_IMGUI)
	set(IMGUI_STDLIB ON)
	add_subdirectory(imgui)
endif()

#FetchContent_MakeAvailable(glm Spdlog Tracy Assimp xxHash)

if(MGN_RENDERER_VULKAN)
	#FetchContent_MakeAvailable(VkBoostrap VulkanMemoryAllocator Volk)
endif()

if(MGN_WINDOW_GLFW)
	#FetchContent_MakeAvailable(GLFW)
elseif(MGN_WINDOW_SDL3)
	#FetchContent_MakeAvailable(SDL3)
endif()

if(MGN_IMGUI)

	if(MGN_WINDOW_GLFW)
		target_link_libraries(imgui PUBLIC glfw)
	elseif(MGN_WINDOW_SDL3)
		target_link_libraries(imgui PUBLIC SDL3::SDL3)
	endif()

	if(MGN_RENDERER_VULKAN)
		find_package(Vulkan REQUIRED)
		target_link_libraries(imgui PUBLIC Vulkan::Vulkan)
	endif()
endif()

