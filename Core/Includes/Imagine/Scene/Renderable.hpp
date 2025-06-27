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
		Renderable() = default;
		virtual ~Renderable() = default;
		Renderable(std::shared_ptr<Mesh> m) : mesh(std::move(m)) {}

		std::shared_ptr<Mesh> mesh;
	};

} // namespace Imagine::Core
