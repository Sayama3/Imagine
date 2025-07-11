//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"

namespace Imagine {

#ifdef MGN_DIST
	using AssetMap = std::unordered_map<AssetHandle, Ref<Asset>>;
#else
	using AssetMap = std::map<AssetHandle, Ref<Asset>>;
#endif

	class AssetManagerBase {
	public:
		inline virtual ~AssetManagerBase() = default;

		[[nodiscard]] virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		[[nodiscard]] virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
		[[nodiscard]] virtual AssetType GetAssetType(AssetHandle handle) const = 0;
		[[nodiscard]] virtual const AssetMetadata &GetMetadata(AssetHandle handle) const = 0;

		virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;
		virtual void UnloadAsset(AssetHandle handle) = 0;
		virtual bool AddAsset(Ref<Asset> asset) = 0;
		virtual bool RemoveAsset(AssetHandle handle) = 0;
	};
} // namespace Imagine
