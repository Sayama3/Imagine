//
// Created by Iannis on 23/06/2025.
//

#pragma once
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/Vertex.hpp"

namespace Imagine::Core {

	struct CPUMesh {
		static CPUMesh LoadExternalModelAsMesh(const std::filesystem::path& p);

		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};

} // namespace Imagine::Core
