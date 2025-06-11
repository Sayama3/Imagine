//
// Created by Iannis on 10/06/2025.
//


#include <assimp/Importer.hpp>
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h> // Output data structure

#include "Imagine/Core/Math.hpp"
#include "Imagine/Vulkan/VulkanInitializer.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"


namespace Imagine::Vulkan {
	namespace Initializer {
		std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadMeshes(VulkanRenderer *engine, const std::filesystem::path &filePath) {
			// Create an instance of the Importer class
			Assimp::Importer importer;

			// And have it read the given file with some example postprocessing
			// Usually - if speed is not the most important aspect for you - you'll
			// probably to request more postprocessing than we do in this example.
			const aiScene *scene = importer.ReadFile(filePath.string(),
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenUVCoords |
			aiProcess_FlipUVs |
			aiProcess_SortByPType);

			// If the import failed, report it
			if (nullptr == scene) {
				MGN_CORE_ERROR(importer.GetErrorString());
				return std::nullopt;
			}

			if (!scene->HasMeshes()) {
				return std::nullopt;
			}

			std::vector<std::shared_ptr<MeshAsset>> meshes;

			// use the same vectors for all meshes so that the memory doesnt reallocate as
			// often
			std::vector<uint32_t> indices;
			std::vector<Vertex> vertices;

			constexpr bool c_OverrideColorVertex = true;

			for (uint64_t i = 0; i < scene->mNumMeshes; ++i) {
				const aiMesh *aiMesh = scene->mMeshes[i];
				if (!aiMesh->HasPositions()) {
					continue;
				}

				MeshAsset mesh;
				mesh.name = aiMesh->mName.C_Str();

				indices.clear();
				vertices.clear();

				vertices.reserve(aiMesh->mNumVertices);
				for (uint64_t i = 0; i < aiMesh->mNumVertices; ++i) {
					vertices.emplace_back();
					const auto &pos = aiMesh->mVertices[i];
					vertices[i].position = {pos.x, pos.y, pos.z};

					if (aiMesh->HasNormals()) {
						const auto &normal = aiMesh->mNormals[i];
						vertices[i].normal = {normal.x, normal.y, normal.z};
					}

					if (aiMesh->HasTextureCoords(0)) {
						const auto &texCoords = aiMesh->mTextureCoords[0][i];
						vertices[i].uv_x = texCoords.x;
						vertices[i].uv_y = texCoords.y;
					}

					if constexpr (c_OverrideColorVertex) {
						vertices[i].color = {vertices[i].normal.x, vertices[i].normal.y, vertices[i].normal.z, 1.0};
					}
					else if (aiMesh->HasVertexColors(0)) {
						const auto &color = aiMesh->mColors[0][i];
						vertices[i].color = {color.r, color.g, color.b, color.a};
					}
				}

				indices.reserve(aiMesh->mNumFaces*3);
				for (uint64_t i = 0; i < aiMesh->mNumFaces; ++i) {
					MGN_CORE_ASSERT(aiMesh->mFaces[i].mNumIndices == 3, "The face wasn't properly triangulated.");
					const aiFace& face = aiMesh->mFaces[i];
					indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
				}

				GeoSurface surface;
				surface.startIndex = 0;
				surface.count = indices.size();

				mesh.surfaces.push_back(surface);

				mesh.meshBuffers = engine->UploadMesh(indices, vertices);

				meshes.emplace_back(std::make_shared<MeshAsset>(std::move(mesh)));
			}

			// We're done. Everything will be cleaned up by the importer destructor
			return meshes;
		}
	} // namespace Initializer
} // namespace Imagine::Vulkan
