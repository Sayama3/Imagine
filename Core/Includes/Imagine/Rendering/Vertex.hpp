//
// Created by Iannis on 23/06/2025.
//

#pragma once

#include "Imagine/Core/Math.hpp"

namespace Imagine {
	struct Vertex {
		glm::vec3 position{0,0,0};
		float uv_x{0};
		glm::vec3 normal{0,1,0};
		float uv_y{0};
		glm::vec4 color{1};
	};
}
