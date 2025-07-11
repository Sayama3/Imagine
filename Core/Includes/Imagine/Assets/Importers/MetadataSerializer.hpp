//
// Created by ianpo on 08/07/2025.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"

namespace Imagine {

	class MetadataSerializer {
	public:
		static std::filesystem::path GetMetadataPath(const std::filesystem::path& assetPath);
		static std::filesystem::path GetMetadataPath(const Path& assetPath);
		static bool HasMetadata(const std::filesystem::path& assetPath);
		static std::optional<AssetMetadata> GetMetadata(const std::filesystem::path& assetPath);
		static AssetMetadata CreateMetadata(Ref<Asset> asset, const Path& assetPath);
		static void WriteMetadata(Ref<Asset> asset, const Path& assetPath);
		static void WriteMetadata(const AssetMetadata& meta);
	};

} // namespace Imagine
