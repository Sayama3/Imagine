//
// Created by Iannis on 23/06/2025.
//

#pragma once

#include "Imagine/Math/Core.hpp"

namespace Imagine {
	// Explicitly write the type for the glm types.
	struct Vertex {
		glm::fvec3 position{0,0,0};
		float uv_x{0};
		glm::fvec3 normal{0,1,0};
		float uv_y{0};
		glm::fvec4 tangent{1,0,0,0};
		glm::fvec4 bitangent{0,0,1,0};
		glm::fvec4 color{1,1,1,1};
	};
}
