//
// Created by ianpo on 30/01/2025.
//

#pragma once

#include "Core.hpp"

namespace Imagine {

	template<glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	struct Ray {
		Ray() = default;
		~Ray() = default;
		Ray(glm::vec<L,T,Q> origin, glm::vec<L,T,Q> direction) : Origin(origin), Direction(direction) {}

		glm::vec<L,T,Q> GetPoint(T t) const {
			return Origin + Direction * t;
		}
		glm::vec<L,T,Q> Origin;
		glm::vec<L,T,Q> Direction;
	};

	template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	struct Plane {
		Plane() = default;
		~Plane() = default;
		Plane(glm::vec<3,T,Q> origin, glm::vec<3,T,Q> normal) : Origin(origin), Normal(normal) {}
		glm::vec<3,T,Q> Origin;
		glm::vec<3,T,Q> Normal;
	};

	template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	struct Circle {
		using Vector = glm::vec<2,T,Q>;
		Circle() = default;
		~Circle() = default;
		Circle(const glm::vec<2,T,Q>& center, T radius) : Center{center}, Radius{std::abs(radius)} {}

		glm::vec<2,T,Q> Center;
		T Radius;
	};

	template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	struct Sphere {
		using Vector = glm::vec<3,T,Q>;
		Sphere() = default;
		~Sphere() = default;
		Sphere(const glm::vec<3,T,Q>& center, T radius) : Center{center}, Radius{std::abs(radius)} {}

		glm::vec<3,T,Q> Center;
		T Radius;
	};


	template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	struct Rect {
		Rect() = default;
		~Rect() = default;
		Rect(T minX, T minY, T maxX, T maxY) : minX(minX), minY(minY), maxX(maxX), maxY(maxY) {}
		Rect(glm::vec<2,T,Q> min, glm::vec<2,T,Q> max) : min(min), max(max) {}

		union {
			struct{T minX,minY,maxX,maxY;};
			struct{glm::vec<2,T,Q> min, max;};
		};

		Vec2 GetCenter() const {
			return (max + min) * T(0.5);
		}
		Vec2 GetSize() const {
			return max - min;
		}
		Vec2 GetRelative(Vec2 globalPos) const {
			return (globalPos - min) / GetSize();
		}
		Vec2 GetGlobal(Vec2 relativePos) const {
			return (relativePos * GetSize()) + min;
		}
		bool IsInside(Vec2 globalPos) const {
			return globalPos.x >= min.x &&
				   globalPos.y >= min.y &&
				   globalPos.x <= max.x &&
				   globalPos.y <= max.y;
		}
	};

}

namespace Imagine {

	typedef glm::qua<Real> Quat;
	typedef glm::vec<4, Real> Vec4;
	typedef glm::vec<3, Real> Vec3;
	typedef glm::vec<2, Real> Vec2;
	typedef glm::vec<1, Real> Vec1;

	typedef glm::mat<4, 4, Real> Mat4x4; typedef Mat4x4 Mat4;
	typedef glm::mat<3, 3, Real> Mat3x3; typedef Mat3x3 Mat3;
	typedef glm::mat<2, 2, Real> Mat2x2; typedef Mat2x2 Mat2;
	typedef glm::mat<3, 2, Real> Mat3x2;

	typedef Ray<3, Real> Ray3;
	typedef Ray<2, Real> Ray2;


}
