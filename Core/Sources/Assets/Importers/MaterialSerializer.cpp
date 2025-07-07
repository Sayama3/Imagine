//
// Created by ianpo on 12/04/2024.
//

#include "Imagine/Assets/Importers/MaterialSerializer.hpp"
#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/ThirdParty/YamlCpp.hpp"


namespace Imagine::Core {
	bool MaterialSerializer::IsMaterial(const std::filesystem::path &path) {
		return path.filename().string().ends_with(".mat.mgn");
	}
	void MaterialSerializer::ExportReadableMaterial(const AssetMetadata &metadata, Ref<CPUMaterial> material) {
		if (!material) return;
		YAML::Emitter out;
		out << *material;
		ThirdParty::YamlCpp::WriteYamlFile(metadata.FilePath.path, out);
	}
	Ref<Asset> MaterialSerializer::ImportReadableMaterial(const AssetMetadata &metadata) {
		if (!FileSystem::Exist(metadata.FilePath)) return nullptr;
		const YAML::Node root = ThirdParty::YamlCpp::ReadFileAsYAML(metadata.FilePath.GetFullPath());
		Ref<CPUMaterial> mat = CreateRef<CPUMaterial>();
		*mat = root.as<CPUMaterial>();
		return mat;
	}

	bool MaterialSerializer::IsMaterialInstance(const std::filesystem::path &path) {
		return path.filename().string().ends_with(".imat.mgn");
	}
	void MaterialSerializer::ExportReadableMaterialInstance(const AssetMetadata &metadata, Ref<CPUMaterialInstance> instance) {
		if (!instance) return;
		YAML::Emitter out;
		out << *instance;
		ThirdParty::YamlCpp::WriteYamlFile(metadata.FilePath.path, out);
	}
	Ref<Asset> MaterialSerializer::ImportReadableMaterialInstance(const AssetMetadata &metadata) {
		if (!FileSystem::Exist(metadata.FilePath)) return nullptr;
		const YAML::Node root = ThirdParty::YamlCpp::ReadFileAsYAML(metadata.FilePath.GetFullPath());
		Ref<CPUMaterialInstance> mat = CreateRef<CPUMaterialInstance>();
		*mat = root.as<CPUMaterialInstance>();
		return mat;
	}
} // namespace Imagine::Core
