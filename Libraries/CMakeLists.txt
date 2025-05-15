cmake_minimum_required(VERSION 3.16)

include(FetchContent)

if(MGN_WINDOW_GLFW)
FetchContent_Declare(
		GLFW
		GIT_REPOSITORY "https://github.com/glfw/glfw.git"
		GIT_TAG "3.4"
		GIT_PROGRESS TRUE
)
endif()

set(GLM_ENABLE_CXX_20 ON)
set(GLM_BUILD_LIBRARY OFF)
set(GLM_ENABLE_FAST_MATH ON)
FetchContent_Declare(
		glm
		GIT_REPOSITORY "https://github.com/g-truc/glm.git"
		GIT_TAG "1.0.1"
)

if(MGN_WINDOW_GLFW)
	set(IMGUI_GLFW ON)
endif()

if(MGN_RENDERER_VULKAN)
	set(IMGUI_VULKAN ON)
endif()

if(MGN_IMGUI)
	set(IMGUI_STDLIB ON)
	FetchContent_Declare(
			ImGui
			GIT_REPOSITORY "https://github.com/Sayama3/imgui.git"
			GIT_TAG "docking"
			GIT_PROGRESS TRUE
	)
endif()

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

#FetchContent_Declare(
#		EASTL
#		GIT_REPOSITORY "https://github.com/electronicarts/EASTL.git"
#		GIT_TAG "3.21.23"
#		GIT_PROGRESS TRUE
#)

FetchContent_MakeAvailable(glm Spdlog Tracy)

if(MGN_WINDOW_GLFW)
	FetchContent_MakeAvailable(GLFW)
endif()

if(MGN_IMGUI)
	FetchContent_MakeAvailable(ImGui)

	if(MGN_WINDOW_GLFW)
		target_link_libraries(imgui PUBLIC glfw)
	endif()

	if(MGN_RENDERER_VULKAN)
		find_package(Vulkan REQUIRED)
		target_link_libraries(imgui PUBLIC Vulkan::Vulkan)
	endif()
endif()
