//
// Created by ianpo on 01/06/2025.
//

#pragma once

#include "Imagine/Rendering/ShaderParameters.hpp"

namespace Imagine::Core {

	class Shader {
	public:
		virtual ~Shader() = default;
		virtual const char *GetName() const = 0;
	};

} // namespace Imagine::Core
