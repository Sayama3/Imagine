cmake_minimum_required(VERSION 3.25)

set(EXTERNAL_CORE_SRC_FILES
		IncludeExternals/stb_image.h
		IncludeExternals/stb_image_resize2.h
		IncludeExternals/stb_image_write.h
		IncludeExternals/stb_perlin.h
		IncludeExternals/stb_sprintf.h
		IncludeExternals/stb_truetype.h
)

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
		Sources/Components/Renderable.cpp
		Includes/Imagine/Components/Renderable.hpp
		Sources/Components/Physicalisable.cpp
		Includes/Imagine/Components/Physicalisable.hpp
		Sources/Core/BufferView.cpp
		Includes/Imagine/Core/BufferView.hpp
		Sources/Core/SparseSet.cpp
		Includes/Imagine/Core/SparseSet.hpp
		Sources/Core/RTTI.cpp
		Includes/Imagine/Core/RTTI.hpp
		Sources/Core/HeapArray.cpp
		Includes/Imagine/Core/HeapArray.hpp
		Includes/Imagine/Core/MemoryHelper.hpp
		Sources/Components/Component.cpp
		Includes/Imagine/Components/Component.hpp
		Sources/Core/RawHeapArray.cpp
		Includes/Imagine/Core/RawHeapArray.hpp
		Sources/Core/RawSparseSet.cpp
		Includes/Imagine/Core/RawSparseSet.hpp
		Includes/Imagine/Events/Event.hpp
		Includes/Imagine/Events/MouseEvent.hpp
		Includes/Imagine/Events/KeyEvent.hpp
		Includes/Imagine/Events/ApplicationEvent.hpp
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
		Includes/Imagine/Rendering/ShaderParameters.hpp
		Includes/Imagine/Rendering/DrawContext.hpp
		Includes/Imagine/Scene/Relationship.hpp
		Sources/Rendering/RenderObject.cpp
		Includes/Imagine/Rendering/RenderObject.hpp
		Sources/Scene/SceneManager.cpp
		Includes/Imagine/Scene/SceneManager.hpp
		Sources/Core/Math.cpp
		Includes/Imagine/Core/MouseButtonCodes.hpp
		Includes/Imagine/Core/KeyCodes.hpp
		Sources/Core/Inputs.cpp
		Includes/Imagine/Core/Inputs.hpp
		Sources/Rendering/CPU/CPUMesh.cpp
		Includes/Imagine/Rendering/CPU/CPUMesh.hpp
		Sources/Rendering/Camera.cpp
		Includes/Imagine/Rendering/Camera.hpp
		Includes/Imagine/Math/Core.hpp
		Includes/Imagine/Math/Geometry.hpp
		Includes/Imagine/Math/MeshGraph2D.hpp
		Includes/Imagine/Math/Shells.hpp
		Includes/Imagine/Math/Types.hpp
		Includes/Imagine/Math/Raycast.hpp
		Includes/Imagine/Math/Triangulation.hpp
		Includes/Imagine/Math/Basics.hpp
		Includes/Imagine/Math/Mesh.hpp
		Includes/Imagine/Rendering/MeshParameters.hpp
		Includes/Imagine/Math/MeshGraph3D.hpp
		Includes/Imagine/Math/ChaikinCurves.hpp
		Sources/Layers/LayerStack.cpp
		Sources/Layers/ImGuiLayer.cpp
		Includes/Imagine/Layers/ImGuiLayer.hpp
		Sources/ThirdParty/Assimp.cpp
		Includes/Imagine/ThirdParty/Assimp.hpp
		Sources/ThirdParty/Stb.cpp
		Includes/Imagine/ThirdParty/Stb.hpp
		Includes/Imagine/Math/Image.hpp
		Sources/Math/Image.cpp
		Includes/Imagine/Core/Profiling.hpp
		Includes/Imagine/Project/Project.hpp
		Includes/Imagine/Project/ProjectSerializer.hpp
		Sources/Project/Project.cpp
		Sources/Project/ProjectSerializer.cpp
		Includes/Imagine/Assets/AssetMetadata.hpp
		Includes/Imagine/Assets/AssetHandle.hpp
		Includes/Imagine/Assets/AssetType.hpp
		Sources/Assets/AssetType.cpp
		Sources/Assets/AssetHandle.cpp
		Includes/Imagine/Assets/AssetManagerBase.hpp
		Sources/Assets/Asset.cpp
		Includes/Imagine/Assets/Asset.hpp
		Includes/Imagine/Assets/AssetField.decl.hpp
		Includes/Imagine/Assets/AssetField.forward.hpp
		Includes/Imagine/Assets/AssetField.hpp
		Includes/Imagine/Assets/AssetField.impl.hpp
		Includes/Imagine/Assets/AssetManager.hpp
		Includes/Imagine/Core/InternalCore.hpp
		Includes/Imagine/Events/CoreEvents.hpp
		Sources/Rendering/CPU/CPUMaterial.cpp
		Includes/Imagine/Rendering/CPU/CPUMaterial.hpp
		Sources/Rendering/CPU/CPUMaterialInstance.cpp
		Includes/Imagine/Rendering/CPU/CPUMaterialInstance.hpp
		Sources/Rendering/MaterialComponents.cpp
		Includes/Imagine/Rendering/MaterialComponents.hpp
		Sources/Rendering/CPU/CPUTexture2D.cpp
		Includes/Imagine/Rendering/CPU/CPUTexture2D.hpp
		Sources/Rendering/CPU/CPUTexture3D.cpp
		Includes/Imagine/Rendering/CPU/CPUTexture3D.hpp
		Sources/Rendering/CPU/CPUShader.cpp
		Includes/Imagine/Rendering/CPU/CPUShader.hpp
		Includes/Imagine/Rendering/GPU/GPUMaterial.hpp
		Includes/Imagine/Rendering/GPU/GPUMaterialInstance.hpp
		Includes/Imagine/Rendering/GPU/GPUTexture2D.hpp
		Includes/Imagine/Rendering/GPU/GPUTexture3D.hpp
		Includes/Imagine/Rendering/GPU/GPUMesh.hpp
		Includes/Imagine/Rendering/VirtualTextures.hpp
		Sources/Assets/FileAssetManager.cpp
		Includes/Imagine/Assets/FileAssetManager.hpp
		Includes/Imagine/Assets/Importers/MaterialSerializer.hpp
		Includes/Imagine/Assets/Importers/MeshImporter.hpp
		Includes/Imagine/Assets/Importers/SceneImporter.hpp
		Includes/Imagine/Assets/Importers/ShaderSerializer.hpp
		Includes/Imagine/Assets/Importers/TextureSerializer.hpp
		Sources/Assets/Importers/MaterialSerializer.cpp
		Sources/Assets/Importers/MeshImporter.cpp
		Sources/Assets/Importers/SceneImporter.cpp
		Sources/Assets/Importers/ShaderSerializer.cpp
		Sources/Assets/Importers/TextureSerializer.cpp
		Includes/Imagine/Assets/ImGui/TextureImGui.hpp
		Includes/Imagine/Assets/ImGui/ShaderImGui.hpp
		Includes/Imagine/Assets/ImGui/SceneImGui.hpp
		Includes/Imagine/Assets/ImGui/MeshImGui.hpp
		Includes/Imagine/Assets/ImGui/MaterialImGui.hpp
		Sources/Assets/ImGui/TextureImGui.cpp
		Sources/Assets/ImGui/ShaderImGui.cpp
		Sources/Assets/ImGui/SceneImGui.cpp
		Sources/Assets/ImGui/MeshImGui.cpp
		Sources/Assets/ImGui/MaterialImGui.cpp
		Includes/Imagine/Math/BoundingBox.hpp
		Includes/Imagine/Math/BoundingObject.hpp
		Includes/Imagine/Math/BoundingSphere.hpp
		Sources/Math/BoundingBox.cpp
		Sources/Math/BoundingSphere.cpp
		Includes/Imagine/Assets/AssetImporter.hpp
		Sources/Assets/AssetImporter.cpp
		Sources/Core/Hash.cpp
		Includes/Imagine/Core/Hash.hpp
		Sources/ThirdParty/YamlCpp.cpp
		Includes/Imagine/ThirdParty/YamlCpp.hpp
		Includes/Imagine/Core/UUID.forward.hpp
		Includes/Imagine/ThirdParty/YamlCpp/YAML_MATH.hpp
		Includes/Imagine/ThirdParty/YamlCpp/YAML_DEFINE.hpp
		Includes/Imagine/ThirdParty/YamlCpp/YAML_ASSET.hpp
		Includes/Imagine/ThirdParty/YamlCpp/YAML_MATERIAL.hpp
		Includes/Imagine/ThirdParty/YamlCpp/YAML_SHADER.hpp
		Includes/Imagine/ThirdParty/YamlCpp/YAML_STD.hpp
		Includes/Imagine/ThirdParty/YamlCpp/YAML_MATERIAL_INSTANCE.hpp
		Includes/Imagine/ThirdParty/YamlCpp/YAML_MESH.hpp
		Includes/Imagine/ThirdParty/YamlCpp/YAML_CORE.hpp
		Includes/Imagine/ThirdParty/YamlCpp/YAML_VIRTUAL_TEXTURE.hpp
		Includes/Imagine/Rendering/CPU/CPUVirtualTexture.hpp
		Includes/Imagine/Rendering/TextureParameters.hpp
		Includes/Imagine/Math/ImagePixelType.hpp
		Sources/Rendering/CPU/CPUVirtualTexture.cpp
		Sources/Assets/Importers/MetadataSerializer.cpp
		Includes/Imagine/Assets/Importers/MetadataSerializer.hpp
		Sources/Rendering/CPU/CPUModel.cpp
		Includes/Imagine/Rendering/CPU/CPUModel.hpp
		Sources/Rendering/Light.cpp
		Includes/Imagine/Rendering/Light.hpp
		Includes/Imagine/Rendering/GPU/GPUSceneData.hpp
		Includes/Imagine/Rendering/GPU/GPULightData.hpp
)

add_library(Core STATIC ${CORE_SRC_FILES} ${EXTERNAL_CORE_SRC_FILES})

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

target_compile_definitions(Core PUBLIC GLM_FORCE_RADIANS=1)
target_compile_definitions(Core PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE=1)
target_compile_definitions(Core PUBLIC GLM_ENABLE_EXPERIMENTAL=1)
target_compile_definitions(Core PUBLIC GLM_FORCE_LEFT_HANDED=1)

target_include_directories(Core PUBLIC IncludeExternals)
target_include_directories(Core PUBLIC Includes)
target_include_directories(Core PRIVATE Sources)

target_compile_features(Core PUBLIC cxx_std_20)

target_link_libraries(Core PUBLIC glm::glm-header-only spdlog::spdlog_header_only TracyClient assimp::assimp sol2::sol2 yaml-cpp::yaml-cpp Jolt)
# target_link_libraries(Core PUBLIC Bitsery::bitsery)
target_link_libraries(Core PRIVATE xxhash)

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