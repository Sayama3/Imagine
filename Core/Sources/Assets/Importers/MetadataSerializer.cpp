//
// Created by ianpo on 08/07/2025.
//

#include "Imagine/Assets/Importers/MetadataSerializer.hpp"

#include "Imagine/ThirdParty/YamlCpp.hpp"

#include <yaml-cpp/yaml.h>

using namespace std::literals;
namespace fs = std::filesystem;

namespace Imagine::Core {
	fs::path MetadataSerializer::GetMetadataPath(const fs::path &assetPath) {
		fs::path meta = assetPath;
		meta += ".meta";
		return std::move(meta);
	}
	fs::path MetadataSerializer::GetMetadataPath(const Path &assetPath) {
		return GetMetadataPath(assetPath.GetFullPath());
	}
	bool MetadataSerializer::HasMetadata(const fs::path& assetPath) {
		return fs::exists(GetMetadataPath(assetPath));
	}
	std::optional<AssetMetadata> MetadataSerializer::GetMetadata(const fs::path &assetPath) {
		MGN_PROFILE_FUNCTION();
		fs::path metaPath = GetMetadataPath(assetPath);
		if (!fs::exists(assetPath)) return std::nullopt;

		const YAML::Node node = ThirdParty::YamlCpp::ReadFileAsYAML(assetPath);
		if (node["Type"].as<std::string>() != "Metadata") {
			return std::nullopt;
		}
		if (!node["Data"].IsMap()) return std::nullopt;
		const AssetMetadata metadata = node["Data"].as<AssetMetadata>();
		return metadata;
	}
	AssetMetadata MetadataSerializer::CreateMetadata(Ref<Asset> asset, const Path &assetPath) {
		MGN_CORE_CASSERT(asset);
		AssetMetadata meta;
		meta.Handle = asset->Handle;
		meta.Type = asset->GetType();
		meta.FilePath = assetPath;
		return std::move(meta);
	}
	void MetadataSerializer::WriteMetadata(Ref<Asset> asset, const Path &assetPath) {
		AssetMetadata metadata = CreateMetadata(asset, assetPath);
		WriteMetadata(metadata);
	}

	void MetadataSerializer::WriteMetadata(const AssetMetadata &meta) {
		YAML::Emitter e;
		e << YAML::BeginMap;
		{
			e << KEYVAL("Type", "Metadata");
			e << KEYVAL("Data", meta);
		}
		e << YAML::EndMap;
		ThirdParty::YamlCpp::WriteYamlFile(GetMetadataPath(meta.FilePath), e);
	}
} // namespace Imagine::Core
