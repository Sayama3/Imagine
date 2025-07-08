//
// Created by Iannis on 10/06/2025.
//


#include "Imagine/ThirdParty/Assimp.hpp"
#include "Imagine/ThirdParty/Stb.hpp"

#include "Imagine/Components/Renderable.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/Math.hpp"
#include "Imagine/Scene/Scene.hpp"

#include "Imagine/Vulkan/VulkanInitializer.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"
#include "Imagine/Vulkan/VulkanImage.hpp"

using namespace Imagine::Core;

namespace Imagine::Vulkan {
	namespace Initializer {
		constexpr bool c_OverrideColorVertex = true;

		std::shared_ptr<AutoDeleteMeshAsset> LoadLines(VulkanRenderer *renderer, std::span<Core::LineObject> lines) {
			std::vector<uint32_t> indices;
			std::vector<Core::Vertex> vertices;

			std::shared_ptr<AutoDeleteMeshAsset> mesh = std::make_shared<AutoDeleteMeshAsset>();

			for (const Core::LineObject &line: lines) {
				if (line.points.size() < 2) continue;
				const uint32_t offset = vertices.size();
				vertices.push_back(line.points.at(0));
				for (uint32_t i = 1; i < line.points.size(); ++i) {
					vertices.push_back(line.points.at(i));
					indices.push_back(offset + (i - 1));
					indices.push_back(offset + i);
				}
			}

			Core::LOD surface;
			surface.index = 0;
			surface.count = indices.size();
			// TODO: Fetch the CPUMaterial and either creater and fetch back the GPUMaterial from it.
			// surface.materialInstance = renderer->GetDefaultLineMaterial();

			mesh->lods.push_back(surface);
			mesh->meshBuffers = renderer->UploadMesh(Core::ConstBufferView::Make(indices), Core::ConstBufferView::Make(vertices));

			return mesh;
		}

		ManualDeleteMeshAsset LoadManualLines(VulkanRenderer *renderer, std::span<Core::LineObject> lines) {
			ManualDeleteMeshAsset mesh;

			std::vector<uint32_t> indices;
			std::vector<Core::Vertex> vertices;


			for (const Core::LineObject &line: lines) {
				if (line.points.size() < 2) continue;
				const uint32_t offset = vertices.size();
				vertices.push_back(line.points.at(0));
				for (uint32_t i = 1; i < line.points.size(); ++i) {
					vertices.push_back(line.points.at(i));
					indices.push_back(offset + (i - 1));
					indices.push_back(offset + i);
				}
			}

			Core::LOD surface;
			surface.index = 0;
			surface.count = indices.size();
			// TODO: Fetch the CPUMaterial and either creater and fetch back the GPUMaterial from it.
			// surface.materialInstance = renderer->GetDefaultLineMaterial();

			mesh.lods.push_back(surface);
			mesh.meshBuffers = renderer->UploadMesh(Core::ConstBufferView::Make(indices), Core::ConstBufferView::Make(vertices));

			return std::move(mesh);
		}

		std::shared_ptr<AutoDeleteMeshAsset> LoadPoints(VulkanRenderer *renderer, std::span<Core::Vertex> points) {
			MGN_CORE_CASSERT(false, "The load points function is not implemented yet.");
			return nullptr;
			/*
						std::vector<uint32_t> indices;
						indices.reserve(points.size());

						std::vector<Core::Vertex> vertices;
						vertices.reserve(points.size());

						std::shared_ptr<AutoDeleteMeshAsset> mesh = std::make_shared<AutoDeleteMeshAsset>();

						for (const auto & point: points) {
							vertices.push_back(point);
							indices.push_back(indices.size());
						}

						Core::LOD surface;
						surface.index = 0;
						surface.count = indices.size();
						surface.material = renderer->GetDefaultPointMaterial();

						mesh->lods.push_back(surface);
						mesh->meshBuffers = renderer->UploadMesh(indices, vertices);

						return mesh;
			*/
		}

		std::optional<std::shared_ptr<AutoDeleteMeshAsset>> LoadCPUMesh(VulkanRenderer *engine, const Core::CPUMesh &cpuMesh) {

			std::shared_ptr<AutoDeleteMeshAsset> mesh = std::make_shared<AutoDeleteMeshAsset>();

			Core::LOD surface = cpuMesh.Lods.front();

			mesh->lods.push_back(surface);

			mesh->meshBuffers = engine->UploadMesh(Core::ConstBufferView::Make(cpuMesh.Indices), Core::ConstBufferView::Make(cpuMesh.Vertices));

			return mesh;
		}

		std::optional<std::vector<std::shared_ptr<AutoDeleteMeshAsset>>> LoadMeshes(VulkanRenderer *engine, const std::filesystem::path &filePath) {
			// Create an instance of the Importer class
			Assimp::Importer importer;

			// And have it read the given file with some example postprocessing
			// Usually - if speed is not the most important aspect for you - you'll
			// probably to request more postprocessing than we do in this example.
			const aiScene *scene = importer.ReadFile(filePath.string(),
													 aiProcess_Triangulate |
															 aiProcess_JoinIdenticalVertices |
															 aiProcess_GenUVCoords |
															 aiProcess_GenSmoothNormals |
															 aiProcess_CalcTangentSpace |
															 aiProcess_ConvertToLeftHanded |
															 aiProcess_SortByPType);

			// If the import failed, report it
			if (nullptr == scene) {
				MGN_CORE_ERROR(importer.GetErrorString());
				return std::nullopt;
			}

			if (!scene->HasMeshes()) {
				return std::nullopt;
			}

			std::vector<std::shared_ptr<AutoDeleteMeshAsset>> meshes;

			// use the same vectors for all meshes so that the memory doesnt reallocate as
			// often
			std::vector<uint32_t> indices;
			std::vector<Core::Vertex> vertices;

			for (uint64_t i = 0; i < scene->mNumMeshes; ++i) {
				const aiMesh *aiMesh = scene->mMeshes[i];
				if (!aiMesh->HasPositions()) {
					continue;
				}

				std::shared_ptr<AutoDeleteMeshAsset> mesh = std::make_shared<AutoDeleteMeshAsset>();
				mesh->name = aiMesh->mName.C_Str();

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
					MGN_CORE_CASSERT(aiMesh->mFaces[i].mNumIndices == 3, "The face wasn't properly triangulated.");
					const aiFace &face = aiMesh->mFaces[i];
					indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
				}

				Core::LOD surface;
				surface.index = 0;
				surface.count = indices.size();

				mesh->lods.push_back(surface);

				mesh->meshBuffers = engine->UploadMesh(Core::ConstBufferView::Make(indices), Core::ConstBufferView::Make(vertices));

				meshes.emplace_back(std::move(mesh));
			}

			// We're done. Everything will be cleaned up by the importer destructor
			return meshes;
		}
	} // namespace Initializer
} // namespace Imagine::Vulkan
