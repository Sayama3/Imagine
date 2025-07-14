cmake_minimum_required(VERSION 3.25)
# Version with "dxc_lib", "dxc_exe", "volk"...

project(Imagine VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT "20")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
set(CPP_RTTI_ENABLED ON)

set(BUILD_SHARED_LIBS OFF)

# No window backend start the application in headless
option(MGN_WINDOW_GLFW "Launch the application with a window and GLFW as a backend" OFF) # Broken for now.
option(MGN_WINDOW_SDL3 "Launch the application with a window and SDL3 as a backend" ON)

# All the windows options are not finished for now. GLFW is currently broken as the input system is not thought out yet.

# No renderer backend start the application in CPU mode.
option(MGN_RENDERER_VULKAN "Launch the application with Vulkan as a render and compute backend." ON)

# IMGUI will automatically be set to OFF if the requirement as GLFW/SDL3/Vulkan/Direct3D/... are not met.
option(MGN_IMGUI "Add ImGui to the Core application" ON)

option(MGN_TESTS "Add Tests" ON)

if(MGN_WINDOW_GLFW)
elseif(MGN_WINDOW_SDL3)
else()
	set(MGN_IMGUI OFF)
endif()

if(MGN_RENDERER_VULKAN)
else()
	set(MGN_IMGUI OFF)
endif()

if (MSVC)
	add_compile_options(/bigobj)
else ()
	add_compile_options(-Wa,-mbig-obj)
endif ()

add_subdirectory(Libraries)
add_subdirectory(Core)
add_subdirectory(Application)

if(MGN_TESTS)
	include(CTest)
	add_subdirectory(Tests)
endif()