//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/DrawContext.hpp"
#include "Imagine/Scene/Entity.hpp"

namespace Imagine::Core {

	//TODO: Move the rendering to ECS like

	struct Renderable {
		virtual ~Renderable() = default;
		virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
	};

} // namespace Imagine::Core
