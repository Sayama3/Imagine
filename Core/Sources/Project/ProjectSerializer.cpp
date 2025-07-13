//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Project/ProjectSerializer.hpp"
#include "Imagine/Project/Project.hpp"
#include "Imagine/ThirdParty/YamlCpp.hpp"

namespace Imagine {

	bool ProjectSerializer::SerializeReadable(const Project *project, const std::filesystem::path &filepath) {
		MGN_PROFILE_FUNCTION();
		YAML::Emitter out;
		auto& v = project->m_Config;
		out << YAML::BeginMap;
		{
			out << KEYVAL("Type", "Project");
			YAML_OUT_VAL_STR("Name", name);
			YAML_OUT_VAL_STR("Asset Directory", assetDirectory);
			YAML_OUT_VAL_STR("Cache Directory", cacheDirectory);
			YAML_OUT_VAL_STR("Scripts Directory", scriptsDirectory);
			YAML_OUT_VAL_STR("Asset Registry Path", assetRegistryPath);
			YAML_OUT_VAL_STR("Start Scene Id", startSceneId);
		}
		out << YAML::EndMap;
		std::filesystem::create_directories(filepath.parent_path());
		ThirdParty::YamlCpp::WriteYamlFile(filepath, out);

		const auto* fileAssetManager = dynamic_cast<const FileAssetManager*>(project->m_AssetManager.get());
		if (fileAssetManager) return FileAssetManagerSerializer::SerializeReadable(fileAssetManager, project->GetAssetRegistry());
		return true;
	}

	Scope<Project> ProjectSerializer::DeserializeReadable(std::filesystem::path filepath) {
		MGN_PROFILE_FUNCTION();
		if (filepath.is_relative()) {
			filepath = std::filesystem::current_path() / filepath;
		}

		if (!std::filesystem::exists(filepath)) return nullptr;

		const YAML::Node node = ThirdParty::YamlCpp::ReadFileAsYAML(filepath);
		if (!node) return nullptr;
		if (node["Type"].as<std::string>() != "Project") return nullptr;

		Scope<Project> project = CreateScope<Project>();
		project->m_ProjectPath = filepath;

		auto& v = project->m_Config;
		v.name = node["Name"].as<std::string>();;
		v.assetDirectory = node["Asset Directory"].as<std::filesystem::path>();;
		v.cacheDirectory = node["Cache Directory"].as<std::filesystem::path>();;
		v.scriptsDirectory = node["Scripts Directory"].as<std::filesystem::path>();;
		v.assetRegistryPath = node["Asset Registry Path"].as<std::filesystem::path>();;
		v.startSceneId = node["Start Scene Id"].as<Imagine::AssetHandle>();;

		project->m_AssetManager = FileAssetManagerSerializer::DeserializeReadable(project->GetAssetRegistry());
		return std::move(project);
	}
} // namespace Imagine
