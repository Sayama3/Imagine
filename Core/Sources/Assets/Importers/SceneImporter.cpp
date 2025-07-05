//
// Created by ianpo on 12/04/2024.
//

#include "Imagine/Assets/Importers/SceneImporter.hpp"
#include "Imagine/Scene/SceneSerializer.hpp"


namespace Imagine::Core
{
	bool SceneImporter::IsScene(const std::filesystem::path &path)
	{
		MGN_PROFILE_FUNCTION();
		std::string extension = path.extension().string();

		std::transform(extension.begin(), extension.end(), extension.begin(), [](char c){ return std::tolower(c);});

		return path.has_extension() && extension == SceneExtension;
	}

	Ref<Asset> SceneImporter::ImportScene(const AssetMetadata& metadata)
	{
		MGN_PROFILE_FUNCTION();
		MGN_CORE_ASSERT(IsScene(metadata.FilePath), "The asset '{0}' is not a scene", metadata.FilePath.string());
		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(metadata.FilePath);
		return scene;
	}

	void SceneImporter::ExportEditorScene(const AssetMetadata& metadata, Ref<Scene> scene)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(metadata.FilePath);
	}
} // namespace Imagine::Core
