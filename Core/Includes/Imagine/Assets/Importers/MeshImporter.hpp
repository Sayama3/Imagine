//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Renderer/Mesh.hpp"
#include "Imagine/Renderer/Model.hpp"
#include <any>


#define MGN_EXTENSION_GLTF ".gltf"
#define MGN_EXTENSION_GLB ".glb"

namespace Imagine::Core
{
	class MeshSerializer
	{
		//TODO: Import using assimp ?
		static const inline std::vector<std::string> ModelExtensions = {MGN_EXTENSION_GLB, MGN_EXTENSION_GLTF};
		static inline const char* const MeshExtension = ".mgn_mesh";
	public:
		static bool IsMesh(const std::filesystem::path& path);
		static void ExportEditorMesh(const AssetMetadata& metadata, Ref<Mesh> mesh);
		static Ref<Asset> ImportMesh(const AssetMetadata& metadata);

		static bool IsModel(const std::filesystem::path& path);
		static void ExportEditorModel(const AssetMetadata& metadata, Ref<Model> model);
		static Ref<Asset> ImportModel(const AssetMetadata& metadata);
	};
} // namespace Imagine::Core

