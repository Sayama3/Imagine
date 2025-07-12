//
// Created by ianpo on 12/04/2024.
//

#include "Imagine/Assets/Importers/MeshImporter.hpp"
#include "Imagine/Assets/Importers/TextureSerializer.hpp"
#include "Imagine/Rendering/Renderer.hpp"

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

		if (auto renderer = Renderer::Get()) {
			for (auto &tex: model->Textures) {
				tex->gpu = renderer->LoadTexture2D(*tex);
			}
			for (Ref<CPUMaterial> &material: model->Materials) {
				material->gpu = renderer->LoadMaterial(*material);
			}
			for (auto &instance: model->Instances) {
				instance->gpu = renderer->LoadMaterialInstance(*instance);
			}
			for (auto &mesh: model->Meshes) {
				mesh->gpu = renderer->LoadMesh(*mesh);
			}
		}

		return model;
	}
} // namespace Imagine
