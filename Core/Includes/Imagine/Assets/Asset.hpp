//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "AssetHandle.hpp"
#include "AssetType.hpp"

#define MGN_IMPLEMENT_ASSET(assetType) inline static constexpr AssetType GetStaticType() {return assetType;} \
inline virtual AssetType GetType() const override {return GetStaticType();}

namespace Imagine {

	class Asset {
	public:
		virtual ~Asset() = default;

		AssetHandle Handle; // Generate handle
		[[nodiscard]] virtual AssetType GetType() const = 0;
	};

} // namespace Imagine
