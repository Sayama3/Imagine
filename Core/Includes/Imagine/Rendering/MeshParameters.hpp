//
// Created by Iannis on 23/06/2025.
//

#pragma once

#include "Imagine/Math/Core.hpp"
#include "Imagine/Assets/AssetHandle.hpp"

namespace Imagine::Core {
	enum class Topology : uint8_t {
		Triangle,
		Line,
		Point,
	};

	// Explicitly write the type for the glm types.
	struct Vertex {
		glm::fvec3 position{0, 0, 0};
		float uv_x{0};
		glm::fvec3 normal{0, 1, 0};
		float uv_y{0};
		glm::fvec4 tangent{1, 0, 0, 0};
		glm::fvec4 bitangent{0, 0, 1, 0};
		glm::fvec4 color{1, 1, 1, 1};
	};

	struct LOD {
		uint32_t index{0};
		uint32_t count{0};
		AssetHandle materialInstance{NULL_ASSET_HANDLE};
	};
} // namespace Imagine::Core

template<typename S>
void serialize(S &s, Imagine::Core::Topology &v) {
	s.value1b(v);
}
template<typename S>
void serialize(S &s, Imagine::Core::LOD &v) {
	s.value4b(v.index);
	s.value4b(v.count);
	s.value<sizeof(Imagine::Core::AssetHandle)>(v.materialInstance);
}

template<typename S>
void serialize(S &s, Imagine::Core::Vertex &v) {
	s.value4b(v.position.x);
	s.value4b(v.position.y);
	s.value4b(v.position.z);

	s.value4b(v.uv_x);
	s.value4b(v.uv_y);

	s.value4b(v.normal.x);
	s.value4b(v.normal.y);
	s.value4b(v.normal.z);

	s.value4b(v.tangent.x);
	s.value4b(v.tangent.y);
	s.value4b(v.tangent.z);

	s.value4b(v.bitangent.x);
	s.value4b(v.bitangent.y);
	s.value4b(v.bitangent.z);

	s.value4b(v.color.r);
	s.value4b(v.color.g);
	s.value4b(v.color.b);
	s.value4b(v.color.a);
}
