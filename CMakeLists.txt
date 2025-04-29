cmake_minimum_required(VERSION 3.25)
# Version with "dxc_lib", "dxc_exe", "volk"...

project(LearningVulkan VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT "20")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
set(CPP_RTTI_ENABLED ON)

set(BUILD_SHARED_LIBS OFF)

# No window backend start the application in headless
option(MGN_WINDOW_GLFW "Launch the application with a window and GLFW as a backend" ON)
#TODO: Implement SDL3 backend
#option(MGN_WINDOW_SDL3 "Launch the application with a window and SDL3 as a backend" ON)

# No renderer backend start the application in CPU mode.
option(MGN_RENDERER_VULKAN "Launch the application with Vulkan as a render and compute backend." ON)

add_subdirectory(Libraries)
add_subdirectory(Core)
add_subdirectory(Application)

message(STATUS "Created target ${LIBRARY_TARGET_NAME}.")