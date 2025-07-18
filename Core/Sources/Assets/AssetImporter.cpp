//
// Created by ianpo on 09/04/2024.
//

#include "Imagine/Assets/AssetImporter.hpp"
#include "Imagine/Assets/Importers/MaterialSerializer.hpp"
#include "Imagine/Assets/Importers/MeshImporter.hpp"
#include "Imagine/Assets/Importers/SceneImporter.hpp"
#include "Imagine/Assets/Importers/ShaderSerializer.hpp"
#include "Imagine/Assets/Importers/TextureSerializer.hpp"
#include "Imagine/Core/InternalCore.hpp"

namespace Imagine
{
	std::unordered_map<AssetType, AssetImportFunction> AssetImporter::AssetLoaders = {
			// {AssetType::Texture2D, TextureSerializer::ImportTexture2D},
			// {AssetType::Texture3D, TextureSerializer::ImportTexture3D},
			// {AssetType::CubeMap, TextureSerializer::ImportCubemap},
			{AssetType::ShaderSource, ShaderSerializer::ImportReadableShaderSource},
			// {AssetType::Mesh, MeshSerializer::ImportMesh},
			// {AssetType::Scene, SceneImporter::ImportScene},
			{AssetType::Material, MaterialSerializer::ImportReadableMaterial},
			{AssetType::MaterialInstance, MaterialSerializer::ImportReadableMaterialInstance},
			{AssetType::Model, MeshSerializer::ImportModel},
	};

	std::unordered_map<AssetType, AssetDetectorFunction> AssetImporter::AssetDetectors = {
			// {AssetType::Texture2D, TextureSerializer::IsTexture2D},
			// {AssetType::Texture3D, TextureSerializer::IsTexture3D},
			// {AssetType::CubeMap, TextureSerializer::IsCubemap},
			{AssetType::ShaderSource, ShaderSerializer::IsShaderSource},
			// {AssetType::Mesh, MeshSerializer::IsMesh},
			// {AssetType::Scene, SceneImporter::IsScene},
			{AssetType::Material, MaterialSerializer::IsMaterial},
			{AssetType::MaterialInstance, MaterialSerializer::IsMaterialInstance},
			{AssetType::Model, MeshSerializer::IsModel},
	};

	Ref<Asset> AssetImporter::ImportAsset(const AssetMetadata &metadata)
	{
		MGN_PROFILE_FUNCTION();
		auto it = AssetLoaders.find(metadata.Type);
		if(it == AssetLoaders.end()) {
			MGN_CORE_ERROR("No importer for type {0}.", metadata.Type);
			return nullptr;
		}
		Ref<Asset> asset = it->second(metadata);
		if(asset) asset->Handle = metadata.Handle;
		return asset;
	}

	AssetType AssetImporter::GetAssetType(const Path &path)
	{
		MGN_PROFILE_FUNCTION();
		auto it = std::find_if(AssetDetectors.begin(), AssetDetectors.end(), [&path](const std::pair<AssetType, AssetDetectorFunction>& pair){return pair.second(path.GetFullPath());});
		if(it == AssetDetectors.end()) {
			return AssetType::None;
		}
		else {
			return it->first;
		}
	}

	std::vector<AssetType> AssetImporter::GetPossibleAssetTypes(const Path &path)
	{
		std::vector<AssetType> result;
		result.reserve(AssetDetectors.size());
		std::for_each(AssetDetectors.begin(), AssetDetectors.end(), [&path, &result](const std::pair<AssetType, AssetDetectorFunction>& pair){
			if(pair.second(path.GetFullPath())) {
				result.push_back(pair.first);
			}
		});
		return result;
	}

	bool AssetImporter::HasAssetType(const Path& path, const AssetType type)
	{
		MGN_PROFILE_FUNCTION();
		if (type == AssetType::None || !AssetDetectors.contains(type)) return false;

		return AssetDetectors.at(type)(path.GetFullPath());
	}
}// namespace Imagine