//
// Created by Iannis on 24/06/2025.
//

#pragma once

#include "Core.hpp"
#include "Types.hpp"

namespace Imagine::Math {

	template<typename T = Real, glm::qualifier Q = glm::defaultp>
	class MeshGraph3D {
	public:
		using vec3 = glm::vec<3, T, Q>;
	public:
		std::vector<vec3> vertices;
	};
}