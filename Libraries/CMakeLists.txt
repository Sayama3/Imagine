cmake_minimum_required(VERSION 3.16)

include(FetchContent)

set(GLM_ENABLE_CXX_20 ON)
set(GLM_BUILD_LIBRARY OFF)
set(GLM_ENABLE_FAST_MATH ON)
FetchContent_Declare(
		glm
		GIT_REPOSITORY "https://github.com/g-truc/glm.git"
		GIT_TAG "1.0.1"
)


FetchContent_Declare(
		Spdlog
		GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
		GIT_TAG "v1.15.3"
		GIT_PROGRESS TRUE
)

set(TRACY_ENABLE ON)
set(TRACY_STATIC ON)
set(TRACY_ON_DEMAND ON)
FetchContent_Declare(
		Tracy
		GIT_REPOSITORY "https://github.com/wolfpld/tracy.git"
		GIT_TAG "v0.11.1"
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
)

set(XXHASH_BUILD_XXHSUM OFF)
FetchContent_Declare(xxHash
		GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
		GIT_TAG "1fc167c19f009265eb0a37117a03bbcdbc687e59" # Commit of the 2 June 2025
		SOURCE_SUBDIR  build/cmake
)

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
FetchContent_Declare(
		Assimp
		GIT_REPOSITORY "https://github.com/assimp/assimp.git"
		GIT_TAG "v6.0.0"
)


set(MESHOPT_BUILD_DEMO OFF) # Build demo
set(MESHOPT_BUILD_GLTFPACK OFF) # Build gltfpack
set(MESHOPT_BUILD_SHARED_LIBS OFF) # Build shared libraries
set(MESHOPT_WERROR OFF) # Treat warnings as errors
set(MESHOPT_INSTALL OFF) # Install library
FetchContent_Declare(
		MeshOptimizer
		GIT_REPOSITORY "https://github.com/zeux/meshoptimizer.git"
		GIT_TAG "v0.23"
)

# ===== Window Libraries =====

if(MGN_WINDOW_GLFW)
	set(IMGUI_GLFW ON)
	FetchContent_Declare(
			GLFW
			GIT_REPOSITORY "https://github.com/glfw/glfw.git"
			GIT_TAG "3.4"
			GIT_PROGRESS TRUE
	)
elseif(MGN_WINDOW_SDL3)
	set(IMGUI_SDL3 ON)
	set(SDL_STATIC ON)
	set(SDL_SHARED OFF)
	FetchContent_Declare(
			SDL3
			GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
			GIT_TAG "release-3.2.14"
			GIT_PROGRESS TRUE
	)
endif()

# ===== Renderer Libraries =====

if(MGN_RENDERER_VULKAN)
	set(SDL_VULKAN ON)
	set(SDL_RENDER_VULKAN ON)
	set(IMGUI_VULKAN ON)

	FetchContent_Declare(
			VkBoostrap
			GIT_REPOSITORY "https://github.com/charles-lunarg/vk-bootstrap.git"
			GIT_TAG "v1.4.305"
			GIT_PROGRESS TRUE
	)

	FetchContent_Declare(
			VulkanMemoryAllocator
			GIT_REPOSITORY "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git"
			GIT_TAG "v3.3.0"
			GIT_PROGRESS TRUE
	)
	if (WIN32)
		set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
	#TODO? Add other VOLK_STATIC_DEFINES
	endif()
	FetchContent_Declare(
			Volk
			GIT_REPOSITORY "https://github.com/zeux/volk.git"
			GIT_TAG "1.4.304"
			GIT_PROGRESS TRUE
	)
endif()

# ===== IMGUI =====

if(MGN_IMGUI)
	set(IMGUI_STDLIB ON)
	FetchContent_Declare(
			ImGui
			GIT_REPOSITORY "https://github.com/Sayama3/imgui.git"
			GIT_TAG "docking"
			GIT_PROGRESS TRUE
	)
endif()

FetchContent_MakeAvailable(glm Spdlog Tracy Assimp xxHash)

if(MGN_RENDERER_VULKAN)
	FetchContent_MakeAvailable(VkBoostrap VulkanMemoryAllocator Volk)
endif()

if(MGN_WINDOW_GLFW)
	FetchContent_MakeAvailable(GLFW)
elseif(MGN_WINDOW_SDL3)
	FetchContent_MakeAvailable(SDL3)
endif()

if(MGN_IMGUI)
	FetchContent_MakeAvailable(ImGui)

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
