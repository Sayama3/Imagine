//
// Created by ianpo on 12/04/2024.
//

#include "Imagine/Assets/Importers/MeshImporter.hpp"
#include "Imagine/Assets/Importers/TextureSerializer.hpp"

#include "Imagine/ThirdParty/Assimp.hpp"

namespace Imagine
{
	bool MeshSerializer::IsMesh(const std::filesystem::path &path) {
		return path.extension() == MeshExtension;
	}

	bool MeshSerializer::IsModel(const std::filesystem::path &path) {
		Assimp::Importer i{};
		const std::string extension = path.extension().string();
		return i.IsExtensionSupported(extension.c_str());
	}

	Ref<Asset> MeshSerializer::ImportModel(const AssetMetadata &metadata) {
		auto model = CPUModel::LoadModel(metadata.FilePath);
		model->Handle = metadata.Handle;
		return model;
	}
} // namespace Imagine
