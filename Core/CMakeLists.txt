cmake_minimum_required(VERSION 3.25)

set(CORE_SRC_FILES
		Includes/Imagine/Core.hpp
		Sources/Core.cpp
		Includes/Imagine/Application/Application.hpp
		Sources/Application/Application.cpp
		Sources/Core/UUID.cpp
		Includes/Imagine/Core/UUID.hpp
		Includes/Imagine/Core/TypeHelper.hpp
		Sources/Core/Logger.cpp
		Includes/Imagine/Core/Logger.hpp
		Includes/Imagine/Core/TimeStep.hpp
		Includes/Imagine/Layers/Layer.hpp
		Includes/Imagine/Layers/LayerStack.hpp
		Includes/Imagine/Application/ApplicationParameters.hpp
		Includes/Imagine/Application/Window.hpp
		Includes/Imagine/Rendering/Renderer.hpp
		Includes/Imagine/Application/WindowParameters.hpp
		Sources/Application/Window.cpp
		Sources/Rendering/Renderer.cpp
		Sources/Scene/Scene.cpp
		Includes/Imagine/Scene/Scene.hpp
		Sources/Scene/Entity.cpp
		Includes/Imagine/Scene/Entity.hpp
		Includes/Imagine/Core/Math.hpp
		Includes/Imagine/Core/Buffer.hpp
		Sources/Core/Buffer.cpp
		Sources/Scene/Renderable.cpp
		Includes/Imagine/Scene/Renderable.hpp
		Sources/Scene/Physicalisable.cpp
		Includes/Imagine/Scene/Physicalisable.hpp
		Sources/Core/BufferView.cpp
		Includes/Imagine/Core/BufferView.hpp
		Sources/Core/SparseSet.cpp
		Includes/Imagine/Core/SparseSet.hpp
		Sources/Core/RTTI.cpp
		Includes/Imagine/Core/RTTI.hpp
		Sources/Core/HeapArray.cpp
		Includes/Imagine/Core/HeapArray.hpp
		Includes/Imagine/Core/MemoryHelper.hpp
		Sources/Scene/Component.cpp
		Includes/Imagine/Scene/Component.hpp
		Sources/Core/RawHeapArray.cpp
		Includes/Imagine/Core/RawHeapArray.hpp
		Sources/Core/RawSparseSet.cpp
		Includes/Imagine/Core/RawSparseSet.hpp
		Includes/Imagine/Events/Event.hpp
		Sources/Layers/Layer.cpp
		Includes/Imagine/Scene/Terrain.hpp
		Sources/Scene/Terrain.cpp
		Sources/Core/FileSystem.cpp
		Includes/Imagine/Core/FileSystem.hpp
		Includes/Imagine/Core/DeletionQueue.hpp
		Includes/Imagine/Core/Size.hpp
		Includes/Imagine/Rendering/RendererParameters.hpp
		Sources/Application/ApplicationParameters.cpp
		Includes/Imagine/Rendering/MgnImGui.hpp
		Sources/Rendering/ImGui.cpp
		Sources/Rendering/Material.cpp
		Includes/Imagine/Rendering/Material.hpp
		Sources/Rendering/PushConstant.cpp
		Includes/Imagine/Rendering/PushConstant.hpp
		Sources/Rendering/Shader.cpp
		Includes/Imagine/Rendering/Shader.hpp
		Includes/Imagine/Rendering/ShaderParameters.hpp
		Includes/Imagine/Rendering/DrawContext.hpp
		Includes/Imagine/Scene/Relationship.hpp
		Sources/Rendering/RenderObject.cpp
		Includes/Imagine/Rendering/RenderObject.hpp
		Sources/Rendering/Mesh.cpp
		Includes/Imagine/Rendering/Mesh.hpp
		Sources/Rendering/Model.cpp
		Includes/Imagine/Rendering/Model.hpp
		Sources/Scene/SceneManager.cpp
		Includes/Imagine/Scene/SceneManager.hpp
		Sources/Core/Math.cpp
		Sources/Core/Inputs.cpp
		Includes/Imagine/Core/Inputs.hpp
)

add_library(Core STATIC ${CORE_SRC_FILES})

target_precompile_headers(Core PUBLIC
		# Multithreading headers
		<thread>
		<atomic>
		<future>
		<condition_variable>
		<mutex>
		<shared_mutex>
		<latch>
		<barrier>

		# Utility stuff
		<functional>
		<utility>
		<algorithm>
		<memory>
		<source_location>
		<chrono>
		<coroutine>
		<bitset>
		<execution>

		# Stream related
		<fstream>
		<iostream>

		# Exception related stuff
		<exception>
		<stdexcept>

		# String & IO manipulation
		<filesystem>
		<string>
		<string_view>

		# Containers
		<deque>
		<array>
		<vector>
		<unordered_set>
		<set>
		<unordered_map>
		<map>
		<any>
		<span>
		<variant>
		<optional>
		<tuple>

		# C-Types Helpers
		<cstdio>
		<cstdlib>
		<cstdint>
		<cstring>
		<cmath>
)

target_compile_definitions(Core PUBLIC GLM_ENABLE_EXPERIMENTAL=1)

target_include_directories(Core PUBLIC IncludesExternals)
target_include_directories(Core PUBLIC Includes)
target_include_directories(Core PRIVATE Sources)

target_compile_features(Core PUBLIC cxx_std_20)

target_link_libraries(Core PUBLIC glm::glm-header-only spdlog::spdlog_header_only TracyClient assimp::assimp xxhash)

add_library(Imagine::Core ALIAS Core)

add_subdirectory(Platform)

target_compile_definitions(Core PUBLIC NOMINMAX=1)

if(MGN_IMGUI)
	target_link_libraries(Core PUBLIC imgui)
	target_compile_definitions(Core PUBLIC MGN_IMGUI=1)
endif()

if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
	target_compile_definitions(Core PUBLIC MGN_DEBUG=1)
elseif(CMAKE_BUILD_TYPE MATCHES "[Rr][Ee][Ll][Ee][Aa][Ss][Ee]")
	target_compile_definitions(Core PUBLIC MGN_RELEASE=1)
elseif(CMAKE_BUILD_TYPE MATCHES "[Dd][Ii][Ss][Tt]")
	target_compile_definitions(Core PUBLIC MGN_RELEASE=1)
	target_compile_definitions(Core PUBLIC MGN_DIST=1)
else()
	message(WARNING "Unknown cmake build type '${CMAKE_BUILD_TYPE}'. Assuming Release mode.")
	target_compile_definitions(Core PUBLIC MGN_RELEASE=1)
endif()