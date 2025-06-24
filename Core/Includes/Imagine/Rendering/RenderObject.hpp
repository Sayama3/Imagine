//
// Created by Iannis on 17/06/2025.
//

#pragma once
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/Material.hpp"
#include "Imagine/Rendering/Mesh.hpp"
#include "Imagine/Rendering/Vertex.hpp"

namespace Imagine::Core {

	struct RenderObject {
		RenderObject() = default;
		virtual ~RenderObject() = default;
		RenderObject(const Mat4& trs, std::shared_ptr<Mesh> m) : transform(trs), mesh(m) {}
		Mat4 transform{};
		std::shared_ptr<Mesh> mesh{nullptr};
		//MaterialInstance *material{nullptr};
	};

	struct LineObject {
		// Mat4 transform;
		std::vector<Vertex> points;
		float width = 1.0f;
	};

	struct PointObject {
		// Mat4 transform;
		Vertex point;
	};

} // namespace Imagine::Core
