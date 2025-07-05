//
// Created by ianpo on 12/04/2024.
//

#include "Imagine/Assets/Importers/MaterialSerializer.hpp"
#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Core/YamlHelper.hpp"


namespace Imagine::Core
{

	bool MaterialSerializer::IsMaterial(const std::filesystem::path &path)
	{
		MGN_PROFILE_FUNCTION();
		std::string extension = path.extension().string();

		std::transform(extension.begin(), extension.end(), extension.begin(), [](char c){ return std::tolower(c);});

		return path.has_extension() && extension == MaterialExtension;
	}

	Ref<Asset> MaterialSerializer::ImportMaterial(const AssetMetadata &metadata)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_ASSERT(IsMaterial(metadata.FilePath), "The asset '{0}' is not a material", metadata.FilePath.string());

		YAML::Node root = FileSystem::ReadFileAsYAML(metadata.FilePath);
		YAML::Node node = root["Material"];
		if(!node) return nullptr;

		Ref<Material> material = CreateRef<Material>();
		material->Deserialize(node);

		return material;
	}

	void MaterialSerializer::ExportEditorMaterial(const AssetMetadata& metadata, Ref<Material> material)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_ASSERT(IsMaterial(metadata.FilePath), "The asset '{0}' is not a material", metadata.FilePath.string());


		YAML::Emitter out;
		out << YAML::BeginMap;
		{
			out << KEYVAL("Material", YAML::BeginMap);
			{
				material->Serialize(out);
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		FileSystem::WriteYamlFile(metadata.FilePath, out);
	}
} // namespace Imagine::Core
