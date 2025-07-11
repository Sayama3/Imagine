//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include <spdlog/spdlog.h>

namespace Imagine {
	enum class AssetType : uint16_t {
		None = 0,
		Scene,

		Texture2D,
		Texture3D,
		CubeMap,

		ShaderSource,
		Material,
		MaterialInstance,

		Model,
		Mesh,

		Script
	};


	static constexpr std::string AssetTypeToString(const AssetType type) {
		switch (type) {
			case AssetType::Scene: return std::string{"Scene"};
			case AssetType::Texture2D: return std::string{"Texture2D"};
			case AssetType::Texture3D: return std::string{"Texture3D"};
			case AssetType::CubeMap: return std::string{"CubeMap"};
			case AssetType::ShaderSource: return std::string{"ShaderSource"};
			case AssetType::Material: return std::string{"Material"};
			case AssetType::MaterialInstance: return std::string{"MaterialInstance"};
			case AssetType::Model: return std::string{"Model"};
			case AssetType::Mesh: return std::string{"Mesh"};
			case AssetType::Script: return std::string{"Scripts"};
			default: return std::string{"Unknown"};
		}
	}

	AssetType AssetTypeFromString(const std::string &assetType);

	bool TryAssetTypeFromString(const std::string &assetType, AssetType &type);

	static inline constexpr std::string AssetTypeToPayloadID(AssetType assetType) {
		switch (assetType) {
			case AssetType::Scene: return "MGN_SCENE";
			case AssetType::Texture2D: return "MGN_TEX2D";
			case AssetType::Texture3D: return "MGN_TEX3D";
			case AssetType::CubeMap: return "MGN_CUBEMAP";
			case AssetType::ShaderSource: return "MGN_SHAD_SRC";
			case AssetType::Material: return "MGN_MAT";
			case AssetType::MaterialInstance: return "MGN_MAT_INST";
			case AssetType::Model: return "MGN_MODEL";
			case AssetType::Mesh: return "MGN_MESH";
			case AssetType::Script: return "MGN_SCRIPT";
			case AssetType::None: return "MGN_ASSET_NONE";
		}
	}

} // namespace Imagine


inline std::ostream &operator<<(std::ostream &os, const Imagine::AssetType &value) {
	return os << Imagine::AssetTypeToString(value);
}

template<>
struct fmt::formatter<Imagine::AssetType> : fmt::formatter<std::string>
{
	auto format(Imagine::AssetType value, format_context &ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", Imagine::AssetTypeToString(value));
	}
};

