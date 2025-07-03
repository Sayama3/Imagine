//
// Created by Iannis on 23/06/2025.
//

#pragma once
#include "Imagine/Rendering/Vertex.hpp"

namespace Imagine::Core {

	struct CPUMesh {
		CPUMesh() = default;
		CPUMesh(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices) : Vertices(std::move(vertices)), Indices(std::move(indices)) {}
		CPUMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) : Vertices(vertices), Indices(indices) {}
		~CPUMesh() = default;
		CPUMesh(const CPUMesh&) = default;
		CPUMesh& operator=(const CPUMesh&) = default;
		CPUMesh(CPUMesh&& o) noexcept {swap(o);}
		CPUMesh& operator=(CPUMesh&& o) noexcept {swap(o); return *this;}

		void swap(CPUMesh& o) noexcept {
			Vertices.swap(o.Vertices);
			Indices.swap(o.Indices);
		}

		static CPUMesh LoadExternalModelAsMesh(const std::filesystem::path& p);

		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};

} // namespace Imagine::Core
