//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Core/SmartPointers.hpp"

namespace Imagine::Core {

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

		virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;
		virtual AssetHandle LoadAsset(Ref<Asset> asset) = 0;
		virtual void UnloadAsset(AssetHandle handle) = 0;
	};
} // namespace Imagine::Core
