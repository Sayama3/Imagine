//
// Created by ianpo on 09/07/2025.
//

#pragma once
#include "Imagine/Math/Core.hpp"
namespace Imagine::Core {

	struct Light {
		glm::fvec4 color;
		glm::fvec4 direction;
		glm::fvec3 position;
		int type;
	};

} // namespace Imagine::Core
