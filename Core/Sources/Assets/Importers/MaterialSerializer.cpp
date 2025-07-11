//
// Created by ianpo on 12/04/2024.
//

#include "Imagine/Assets/Importers/MaterialSerializer.hpp"
#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/ThirdParty/YamlCpp.hpp"


namespace Imagine {
	bool MaterialSerializer::IsMaterial(const std::filesystem::path &path) {
		return path.filename().string().ends_with(".mat.mgn");
	}
	void MaterialSerializer::ExportReadableMaterial(const AssetMetadata &metadata, Ref<CPUMaterial> material) {
		if (!material) return;
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << KEYVAL("Type", "Material");
		out << KEYVAL("Data", *material);
		out << YAML::EndMap;
		ThirdParty::YamlCpp::WriteYamlFile(metadata.FilePath, out);
	}
	Ref<Asset> MaterialSerializer::ImportReadableMaterial(const AssetMetadata &metadata) {
		if (!FileSystem::Exist(metadata.FilePath)) return nullptr;
		const YAML::Node root = ThirdParty::YamlCpp::ReadFileAsYAML(metadata.FilePath.GetFullPath());

		if (root["Type"] && root["Type"].as<std::string>() == "Material") {
			Ref<CPUMaterial> mat = CreateRef<CPUMaterial>();
			*mat = root["Data"].as<CPUMaterial>();
			mat->Handle = metadata.Handle;
			return mat;
		}

		return nullptr;
	}

	bool MaterialSerializer::IsMaterialInstance(const std::filesystem::path &path) {
		return path.filename().string().ends_with(".imat.mgn");
	}
	void MaterialSerializer::ExportReadableMaterialInstance(const AssetMetadata &metadata, Ref<CPUMaterialInstance> instance) {
		if (!instance) return;
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << KEYVAL("Type", "Material Instance");
		out << KEYVAL("Data", *instance);
		out << YAML::EndMap;
		ThirdParty::YamlCpp::WriteYamlFile(metadata.FilePath, out);
	}
	Ref<Asset> MaterialSerializer::ImportReadableMaterialInstance(const AssetMetadata &metadata) {
		if (!FileSystem::Exist(metadata.FilePath)) return nullptr;
		const YAML::Node root = ThirdParty::YamlCpp::ReadFileAsYAML(metadata.FilePath.GetFullPath());

		if (root["Type"] && root["Type"].as<std::string>() == "Material Instance") {
			Ref<CPUMaterialInstance> mat = CreateRef<CPUMaterialInstance>();
			*mat = root["Data"].as<CPUMaterialInstance>();
			mat->Handle = metadata.Handle;
			return mat;
		}

		return nullptr;
	}
} // namespace Imagine
