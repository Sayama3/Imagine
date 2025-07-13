//
// Created by Iannis on 23/06/2025.
//

#include "Imagine/Rendering/CPU/CPUMesh.hpp"

#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/Renderer.hpp"

#include "Imagine/ThirdParty/Assimp.hpp"

namespace Imagine {
	CPUMesh::CPUMesh(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices) :
		Vertices(std::move(vertices)), Indices(std::move(indices)) {CalcAABB();}
	CPUMesh::CPUMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) :
		Vertices(vertices), Indices(indices) { CalcAABB(); }

	CPUMesh::CPUMesh(const std::vector<Vertex> &vertices) :
		Vertices(vertices), Indices(Vertices.size()) {
		if (Vertices.empty()) return;
		aabb.SetMinMax(Vertices.begin()->position, Vertices.begin()->position);
		for (uint32_t i = 0; i < Indices.size(); ++i) {
			Indices.push_back(i);
			aabb.Grow(Vertices[i].position);
		}
	}
	CPUMesh::CPUMesh(std::vector<Vertex> &&vertices) :
		Vertices(std::move(vertices)), Indices(Vertices.size()) {
		if (Vertices.empty()) return;
		aabb.SetMinMax(Vertices.begin()->position, Vertices.begin()->position);
		for (uint32_t i = 0; i < Indices.size(); ++i) {
			Indices.push_back(i);
			aabb.Grow(Vertices[i].position);
		}
	}

	CPUMesh::CPUMesh(CPUMesh &&o) noexcept { swap(o); }
	CPUMesh &CPUMesh::operator=(CPUMesh &&o) noexcept {
		swap(o);
		return *this;
	}
	void CPUMesh::swap(CPUMesh &o) noexcept {
		Name.swap(o.Name);
		Vertices.swap(o.Vertices);
		Indices.swap(o.Indices);
		Lods.swap(o.Lods);
		std::swap(aabb, o.aabb);
	}

	CPUMesh CPUMesh::LoadExternalModelAsMesh(const std::filesystem::path &p) {
		CPUMesh finalMesh;
		finalMesh.Name = p.stem().string();

		using namespace Imagine;
		Assimp::Importer importer;

		const aiScene *scene = importer.ReadFile(p.string(), ThirdParty::Assimp::GetSmoothPostProcess());

		// If the import failed, report it
		if (nullptr == scene) {
			MGN_CORE_ERROR(importer.GetErrorString());
			return finalMesh;
		}

		std::vector<CPUMesh> meshes;
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

				CPUMesh mesh;

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

					if (aiMesh->HasVertexColors(0)) {
						aiColor4D &color = aiMesh->mColors[0][i];
						vertices[i].color = glm::vec4{color.r, color.g, color.b, color.a};
					}
				}

				indices.reserve(aiMesh->mNumFaces * 3);
				for (uint64_t i = 0; i < aiMesh->mNumFaces; ++i) {
					MGN_CORE_CASSERT(aiMesh->mFaces[i].mNumIndices == 3, "The face wasn't properly triangulated.");
					const aiFace &face = aiMesh->mFaces[i];
					indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
				}

				meshes.emplace_back(vertices, indices);
			}
		}

		// Load the nodes as entity in the scene.
		{
			// Unwrapping everything in a vector to avoid recursion.
			std::vector<std::tuple<aiNode *, aiMatrix4x4>> nodes{{scene->mRootNode, aiMatrix4x4{}}};
			nodes.reserve(scene->mNumMeshes);
			while (!nodes.empty()) {
				// Pop the last node available
				auto [node, parentMatrix] = nodes.back();
				nodes.pop_back();

				const aiMatrix4x4 &globalMatrix = node->mTransformation * parentMatrix;

				if (node->mNumMeshes > 0) {
					// Going from ASSIMP to GLM is going from row major to column major.
					//  Therefore, I create the glm matrix by rotating the matrix and
					//  setting the data properly using the constructor with 4 column
					//  (or vec4 basically) to be explicit and not have any issue later on.
					const glm::mat4 glmGlobalMatrix{
							{globalMatrix.a1, globalMatrix.b1, globalMatrix.c1, globalMatrix.d1}, // Col 1
							{globalMatrix.a2, globalMatrix.b2, globalMatrix.c2, globalMatrix.d2}, // col 2
							{globalMatrix.a3, globalMatrix.b3, globalMatrix.c3, globalMatrix.d3}, // col 3
							{globalMatrix.a4, globalMatrix.b4, globalMatrix.c4, globalMatrix.d4}, // col 4
					};

					for (int i = 0; i < node->mNumMeshes; ++i) {
						const uint32_t vertexOffset = finalMesh.Vertices.size();
						const auto &mesh = meshes[node->mMeshes[i]];
						for (const auto &vertex: mesh.Vertices) {
							Vertex globalVertex = vertex;
							globalVertex.position = glmGlobalMatrix * Vec4(globalVertex.position, 1);
							globalVertex.normal = glmGlobalMatrix * Vec4(globalVertex.normal, 0);
							finalMesh.Vertices.push_back(globalVertex);
						}

						for (const auto indice: mesh.Indices) {
							finalMesh.Indices.push_back(indice + vertexOffset);
						}
					}
				}

				for (int i = 0; i < node->mNumChildren; ++i) {
					aiNode *child = node->mChildren[i];
					nodes.push_back({child, globalMatrix});
				}
			}
		}

		finalMesh.Lods.emplace_back(0, static_cast<uint32_t>(finalMesh.Indices.size()));
		return finalMesh;
	}
	void CPUMesh::LoadMeshInGPU() {
		if (!gpu) {
			if (auto* renderer = Renderer::Get()) {
				gpu = renderer->LoadMesh(*this);
			}
		}
	}

	void CPUMesh::CalcAABB() {
		if (Vertices.empty()) {
			aabb = {{0,0,0},{0,0,0}};
			return;
		}

		aabb.SetMinMax(Vertices.begin()->position, Vertices.begin()->position);

		for (const auto & Vertice : Vertices) {
			aabb.Grow(Vertice.position);
		}
	}
} // namespace Imagine
