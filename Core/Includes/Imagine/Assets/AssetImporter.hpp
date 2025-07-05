//
// Created by ianpo on 09/04/2024.
//

#pragma once

#include "Asset.hpp"
#include "AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"

namespace Imagine::Core
{
	using AssetImportFunction = std::function<Ref<Asset>(const AssetMetadata&)>;
	using AssetDetectorFunction = std::function<bool(const std::filesystem::path&)>;
	class AssetImporter
	{
	public:
		static AssetType GetAssetType(const Path& path);
		static std::vector<AssetType> GetPossibleAssetTypes(const Path& path);
		static bool HasAssetType(const Path& path, AssetType type);
		static Ref<Asset> ImportAsset(const AssetMetadata& metadata);
	private:
		static std::unordered_map<AssetType, AssetImportFunction> AssetLoaders;
		static std::unordered_map<AssetType, AssetDetectorFunction> AssetDetectors;
	};

} // namespace Imagine::Core

