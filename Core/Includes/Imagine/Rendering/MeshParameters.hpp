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

	static inline constexpr std::string TopologyToString(const Topology val) {
		switch (val) {
			case Topology::Triangle: return "Triangle";
			case Topology::Line: return "Line";
			case Topology::Point: return "Point";
		}
		return "Unknown";
	}
	static inline constexpr bool TryTopologyFromString(const std::string& str, Topology& val) {
		if(str == "Triangle") {val = Topology::Triangle; return true;}
		if(str == "Line") {val = Topology::Line; return true;}
		if(str == "Point") {val = Topology::Point; return true;}
		return false;
	}

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

