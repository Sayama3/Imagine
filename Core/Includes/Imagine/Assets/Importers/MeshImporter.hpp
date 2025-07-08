//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUMesh.hpp"
// #include "Imagine/Rendering/CPU/CPUModel.hpp"


#define MGN_EXTENSION_GLTF ".gltf"
#define MGN_EXTENSION_GLB ".glb"

namespace Imagine::Core
{
	class MeshSerializer
	{
		//TODO: Import using assimp ?
		static inline constexpr std::array<const char* const, 2> ModelExtensions{MGN_EXTENSION_GLB, MGN_EXTENSION_GLTF};
		static inline const char* const MeshExtension = ".mgn";
	public:
		static bool IsMesh(const std::filesystem::path& path);
		static void ExportEditorMesh(const AssetMetadata& metadata, Ref<CPUMesh> mesh);
		static Ref<Asset> ImportMesh(const AssetMetadata& metadata);

		static bool IsModel(const std::filesystem::path& path);
		static void ExportEditorModel(const AssetMetadata& metadata, Ref<CPUModel> model);
		static Ref<Asset> ImportModel(const AssetMetadata& metadata);
	};
} // namespace Imagine::Core

