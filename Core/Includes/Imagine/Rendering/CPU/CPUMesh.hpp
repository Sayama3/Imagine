//
// Created by Iannis on 23/06/2025.
//

#pragma once
#include "Imagine/Rendering/Vertex.hpp"

namespace Imagine::Core {

	struct CPUMesh {
	public:
		CPUMesh() = default;
		CPUMesh(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices);
		CPUMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
		~CPUMesh() = default;
		CPUMesh(const CPUMesh &) = default;
		CPUMesh &operator=(const CPUMesh &) = default;
		CPUMesh(CPUMesh &&o) noexcept;
		CPUMesh &operator=(CPUMesh &&o) noexcept;
		void swap(CPUMesh &o) noexcept;

	public:
		static CPUMesh LoadExternalModelAsMesh(const std::filesystem::path &p);

	public:
		struct LOD {
			uint32_t index;
			uint32_t count;
		};

	public:
		std::string Name;
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
		std::vector<LOD> Lods;
	};

} // namespace Imagine::Core
