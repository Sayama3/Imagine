//
// Created by Iannis on 23/06/2025.
//

#pragma once
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Math/BoundingBox.hpp"
#include "Imagine/Rendering/GPU/GPUMesh.hpp"
#include "Imagine/Rendering/MeshParameters.hpp"

namespace Imagine {

	struct CPUMesh final : public Asset {
		MGN_IMPLEMENT_ASSET(AssetType::Mesh);

	public:
		CPUMesh() = default;
		CPUMesh(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices);
		CPUMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
		CPUMesh(const std::vector<Vertex> &vertices);
		CPUMesh(std::vector<Vertex> &&vertices);
		virtual ~CPUMesh() override = default;
		CPUMesh(const CPUMesh &) = default;
		CPUMesh &operator=(const CPUMesh &) = default;
		CPUMesh(CPUMesh &&o) noexcept;
		CPUMesh &operator=(CPUMesh &&o) noexcept;
		void swap(CPUMesh &o) noexcept;

	public:
		static CPUMesh LoadExternalModelAsMesh(const std::filesystem::path &p);

		void LoadMeshInGPU();
		void CalcAABB();
	public:
		std::string Name;
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
		std::vector<LOD> Lods;
		BoundingBox aabb;
		Ref<GPUMesh> gpu{nullptr};
	};

} // namespace Imagine
