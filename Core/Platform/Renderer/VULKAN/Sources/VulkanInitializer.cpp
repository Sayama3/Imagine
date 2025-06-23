//
// Created by Iannis on 10/06/2025.
//


#include <assimp/Importer.hpp>
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h> // Output data structure

#include "Imagine/Core/Math.hpp"
#include "Imagine/Vulkan/VulkanInitializer.hpp"

#include "Imagine/Scene/Renderable.hpp"
#include "Imagine/Scene/Scene.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"


namespace Imagine::Vulkan {
	namespace Initializer {
		constexpr bool c_OverrideColorVertex = true;

		std::optional<std::shared_ptr<MeshAsset>> LoadModelAsMesh(VulkanRenderer *engine, const std::filesystem::path &filePath) {
			return std::nullopt;
		}

		void LoadModelAsDynamic(VulkanRenderer *engine, Core::Scene *coreScene, Core::EntityID parent, const std::filesystem::path &filePath) {
			using namespace Imagine::Core;
			Assimp::Importer importer;

			const aiScene *scene = importer.ReadFile(filePath.string(),
													 aiProcess_Triangulate |
															 aiProcess_JoinIdenticalVertices |
															 aiProcess_GenUVCoords |
															 aiProcess_FlipUVs |
															 aiProcess_ConvertToLeftHanded |
															 aiProcess_SortByPType);

			// If the import failed, report it
			if (nullptr == scene) {
				MGN_CORE_ERROR(importer.GetErrorString());
				return;
			}

			// TODO: Load the textures first.

			// TODO: Load the material seconds.

			std::vector<std::shared_ptr<MeshAsset>> meshes;
			meshes.reserve(scene->mNumMeshes);

			// Load the meshes
			// TODO: Reuuse the materials for the meshes.
			{
				// use the same vectors for all meshes so that the memory doesnt reallocate as
				// often
				std::vector<uint32_t> indices;
				std::vector<Vertex> vertices;

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

					indices.reserve(aiMesh->mNumFaces * 3);
					for (uint64_t i = 0; i < aiMesh->mNumFaces; ++i) {
						MGN_CORE_ASSERT(aiMesh->mFaces[i].mNumIndices == 3, "The face wasn't properly triangulated.");
						const aiFace &face = aiMesh->mFaces[i];
						indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
					}

					Mesh::LOD surface;
					surface.index = 0;
					surface.count = indices.size();
					surface.material = engine->GetDefaultMaterial();

					mesh.lods.push_back(surface);

					mesh.meshBuffers = engine->UploadMesh(indices, vertices);

					engine->PushDeletion(mesh.meshBuffers.indexBuffer.allocation, mesh.meshBuffers.indexBuffer.buffer);
					engine->PushDeletion(mesh.meshBuffers.vertexBuffer.allocation, mesh.meshBuffers.vertexBuffer.buffer);

					meshes.emplace_back(std::make_shared<MeshAsset>(std::move(mesh)));
				}
			}

			// Load the nodes as entity in the scene.
			{
				// Unwrapping everything in a vector to avoid recursion.
				std::vector<std::tuple<aiNode *, Core::EntityID>> nodes{{scene->mRootNode, parent}};
				nodes.reserve(scene->mNumMeshes);
				while (!nodes.empty()) {
					// Pop the last node available
					auto [node, parentEntityID] = nodes.back();
					nodes.pop_back();

					const char *cName = node->mName.C_Str();

					const aiMatrix4x4 &localMatrix = node->mTransformation; // * parentMatrix;

					// Going from ASSIMP to GLM is going from row major to column major.
					//  Therefore, I create the glm matrix by rotating the matrix and
					//  setting the data properly using the constructor with 4 column
					//  (or vec4 basically) to be explicit and not have any issue later on.
					const glm::mat4 glmLocalMatrix{
							{localMatrix.a1, localMatrix.b1, localMatrix.c1, localMatrix.d1}, // Col 1
							{localMatrix.a2, localMatrix.b2, localMatrix.c2, localMatrix.d2}, // col 2
							{localMatrix.a3, localMatrix.b3, localMatrix.c3, localMatrix.d3}, // col 3
							{localMatrix.a4, localMatrix.b4, localMatrix.c4, localMatrix.d4}, // col 4
					};
					glm::vec3 pos;
					glm::quat rot;
					glm::vec3 scale;
					glm::vec3 skew;
					glm::vec4 perspective;
					glm::decompose(glmLocalMatrix, scale, rot, pos, skew, perspective);

					MGN_CORE_ASSERT(skew == glm::vec3(0), "skew transformation not supported.");
					MGN_CORE_ASSERT(perspective == glm::vec4(0, 0, 0, 1), "perspective transformation not supported.");

					if (scale.x != scale.y || scale.y != scale.z) {
						MGN_CORE_WARN("The scale of the node {} in the model {} is non-uniform. It's preferred to have a uniform scale of 1.", cName, filePath.string());
					}
					else if (Math::SqrMagnitude(scale) != 1) {
						// MGN_CORE_WARN("The scale of the node {} in the model {} is not 1. It's preferred to have a uniform scale of 1.");
					}

					EntityID entityId = coreScene->CreateEntity(parentEntityID);
					coreScene->SetName(entityId, node->mName.C_Str());
					coreScene->GetEntity(entityId).LocalPosition = pos;
					coreScene->GetEntity(entityId).LocalRotation = rot;
					coreScene->GetEntity(entityId).LocalScale = scale;

					if (node->mNumMeshes == 1) {
						coreScene->AddComponent<Renderable>(entityId, std::static_pointer_cast<Mesh>(meshes[*node->mMeshes]));
					}
					else if (node->mNumMeshes > 1) {
						for (int i = 0; i < node->mNumMeshes; ++i) {
							std::shared_ptr<MeshAsset> mesh = meshes[node->mMeshes[i]];
							EntityID meshChild = coreScene->CreateEntity(entityId);
							coreScene->SetName(meshChild, mesh->name);
							coreScene->AddComponent<Renderable>(meshChild, std::static_pointer_cast<Mesh>(mesh));
						}
					}


					for (int i = 0; i < node->mNumChildren; ++i) {
						aiNode *child = node->mChildren[i];
						nodes.push_back({child, entityId});
					}
				}
			}
		}

		std::optional<std::shared_ptr<MeshAsset>> LoadCPUMesh(VulkanRenderer *engine, const Core::CPUMesh& cpuMesh) {

			std::shared_ptr<MeshAsset> mesh = std::make_shared<MeshAsset>();

			Core::Mesh::LOD surface;
			surface.index = 0;
			surface.count = cpuMesh.Indices.size();
			surface.material = engine->GetDefaultMaterial();

			mesh->lods.push_back(surface);

			std::vector<uint32_t>& indices = (std::vector<uint32_t>&)(cpuMesh.Indices);
			std::vector<Vertex>& vertices = (std::vector<Vertex>&)(cpuMesh.Vertices);
			mesh->meshBuffers = engine->UploadMesh(indices, vertices);

			engine->PushDeletion(mesh->meshBuffers.indexBuffer.allocation,mesh->meshBuffers.indexBuffer.buffer);
			engine->PushDeletion(mesh->meshBuffers.vertexBuffer.allocation,mesh->meshBuffers.vertexBuffer.buffer);

			return mesh;
		}

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
															 // aiProcess_ConvertToLeftHanded |
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

				indices.reserve(aiMesh->mNumFaces * 3);
				for (uint64_t i = 0; i < aiMesh->mNumFaces; ++i) {
					MGN_CORE_ASSERT(aiMesh->mFaces[i].mNumIndices == 3, "The face wasn't properly triangulated.");
					const aiFace &face = aiMesh->mFaces[i];
					indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
				}

				Core::Mesh::LOD surface;
				surface.index = 0;
				surface.count = indices.size();

				mesh.lods.push_back(surface);

				mesh.meshBuffers = engine->UploadMesh(indices, vertices);

				engine->PushDeletion(mesh.meshBuffers.indexBuffer.allocation,mesh.meshBuffers.indexBuffer.buffer);
				engine->PushDeletion(mesh.meshBuffers.vertexBuffer.allocation,mesh.meshBuffers.vertexBuffer.buffer);

				meshes.emplace_back(std::make_shared<MeshAsset>(std::move(mesh)));
			}

			// We're done. Everything will be cleaned up by the importer destructor
			return meshes;
		}
	} // namespace Initializer
} // namespace Imagine::Vulkan
