//
// Created by ianpo on 30/01/2025.
//

#pragma once

#include "Basics.hpp"
#include "Geometry.hpp"

namespace Imagine::Math {
	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	static inline std::optional<T> Raycast(const Plane<T,Q>& plane, const Ray<3,T,Q>& ray) {

		const auto localPlane = Plane<T,Q>({0,0,0}, plane.Normal);
		const auto localRay = Ray<3,T,Q>(ray.Origin - plane.Origin, ray.Direction);

		const float denom = Dot(localPlane.Normal, localRay.Direction);

		if (std::abs(denom) <= static_cast<T>(1e-4f)) {
			return std::nullopt;
		}

		const auto t = -(Dot(localPlane.Normal, localRay.Origin)) / denom;

		return t;
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	static inline std::optional<glm::vec<3,T,Q>> RaycastToPoint(const Plane<T,Q>& plane, const Ray<3,T,Q>& ray) {
		const std::optional<T> result = Raycast(plane, ray);
		if (result.has_value()) return ray.GetPoint(result.value());
		return std::nullopt;
	}
}