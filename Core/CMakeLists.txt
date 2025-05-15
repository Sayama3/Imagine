cmake_minimum_required(VERSION 3.25)

set(CORE_SRC_FILES
		Includes/Core.hpp
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
		Includes/Imagine/Application/Renderer.hpp
		Includes/Imagine/Application/WindowParameters.hpp
		Sources/Application/Window.cpp
		Sources/Application/Renderer.cpp
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

		# Exception related stuff
		<exception>
		<stdexcept>

		# String & IO manipulation
		<filesystem>
		<string>
		<string_view>

		# Containers
		<array>
		<vector>
		<unordered_set>
		<set>
		<unordered_map>
		<map>
		<any>
		<variant>
		<optional>
		<tuple>

		# C-Types Helpers
		<cstdint>
		<cstring>
		<cmath>
)

target_compile_definitions(Core PUBLIC GLM_ENABLE_EXPERIMENTAL=1)

target_include_directories(Core PUBLIC Includes)
target_include_directories(Core PRIVATE Sources)

target_compile_features(Core PUBLIC cxx_std_20)

target_link_libraries(Core PUBLIC glm::glm-header-only spdlog::spdlog_header_only TracyClient)

add_library(Imagine::Core ALIAS Core)

add_subdirectory(Platform)

if(MGN_IMGUI AND NOT MGN_NO_IMGUI)
	message(NOTICE "Add ImGui")
	target_link_libraries(Core PUBLIC imgui)
else()
	message(NOTICE "Don't have ImGui")
endif()


if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
	target_compile_definitions(Core PUBLIC MGN_DEBUG=1)
elseif(CMAKE_BUILD_TYPE MATCHES "[Rr][Ee][Ll][Ee][Aa][Ss][Ee]")
	target_compile_definitions(Core PUBLIC MGN_RELEASE=1)
elseif(CMAKE_BUILD_TYPE MATCHES "[Dd][Ii][Ss][Tt]")
	target_compile_definitions(Core PUBLIC MGN_RELEASE=1)
	target_compile_definitions(Core PUBLIC MGN_DIST=1)
else()
	message(FATAL_ERROR "Unknown cmake build type '${CMAKE_BUILD_TYPE}'")
endif()