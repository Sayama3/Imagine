//
// Created by Iannis on 17/06/2025.
//

#pragma once
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/Material.hpp"
#include "Imagine/Rendering/Mesh.hpp"

namespace Imagine::Core {

	struct RenderObject {
		RenderObject() = default;
		virtual ~RenderObject() = default;

		glm::mat4 transform{};
		std::unique_ptr<Mesh> mesh{nullptr};
		MaterialInstance *material{nullptr};
	};

} // namespace Imagine::Core
