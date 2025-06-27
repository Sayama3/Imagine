//
// Created by ianpo on 26/01/2025.
//

#pragma once

#include "Types.hpp"

namespace Imagine::Math {
	template<typename T>
	[[nodiscard]] inline static T Sign(T value) {
		return value < static_cast<T>(0) ? static_cast<T>(-1) : static_cast<T>(1);
	}

	template<glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static glm::mat<L,L,T,Q> Inverse(const glm::mat<L,L,T,Q>& mat) {
		return glm::inverse(mat);
	}

	template<glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static T Determinant(const glm::mat<L,L,T,Q>& mat) {
		return glm::determinant(mat);
	}

	template<glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static bool Approx(const glm::vec<L,T,Q>& a, const glm::vec<L,T,Q>& b) {
		bool same = true;
		for (glm::length_t i = 0; i < L; ++i) {
			same &= Abs(a[i] - b[i]) < std::numeric_limits<T>::epsilon();
		}
		return same;
	}

	/**
	 * Calculate the angle in radians between the vector 1 & 2.
	 * @tparam L Length of the vector.
	 * @tparam T Type of the vector
	 * @tparam Q Alignment of the vector.
	 * @param x Vector 1.
	 * @param y Vector 2.
	 * @return Angle in radians between the vector 1 & 2.
	 */
	template<glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static T Angle(glm::vec<L,T,Q> x, glm::vec<L,T,Q> y) {
		x = glm::normalize(x);
		y = glm::normalize(y);
		return glm::acos(Dot(x,y));
	}

	template<glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static T Magnitude2(const glm::vec<L,T,Q>& x) {
		return glm::length2(x);
	}

	template<glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static T Magnitude(const glm::vec<L,T,Q>& x) {
		return glm::length(x);
	}

	template<glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static T Distance2(const glm::vec<L,T,Q>& a, const glm::vec<L,T,Q>& b) {
		return Magnitude2(b - a);
	}

	template<glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static T Distance(const glm::vec<L,T,Q>& a, const glm::vec<L,T,Q>& b) {
		return Magnitude(b - a);
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static T Magnitude2(const glm::qua<T,Q>& x) {
		return glm::length2(x);
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static T Magnitude(const glm::qua<T,Q>& x) {
		return glm::length(x);
	}

	template<glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static GLM_VEC_T(L,T,Q) Normalize(const glm::vec<L,T,Q>& x) {
		return glm::normalize(x);
	}

	template<glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	inline static void NormalizeInPlace(glm::vec<L,T,Q>& x) {
		const T len = Magnitude(x);
		x /= len;
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static glm::qua<T,Q> Normalize(const glm::qua<T,Q>& x) {
		return glm::normalize(x);
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	inline static void NormalizeInPlace(glm::qua<T,Q>& x) {
		const T len = Magnitude(x);
		x /= len;
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static glm::qua<T,Q> LookAt(const glm::vec<3,T,Q>& forward, const glm::vec<3,T,Q>& up) {
		return glm::quatLookAt(forward, up);
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static glm::qua<T,Q> MakeQuat(const T radians, const glm::vec<3,T,Q>& axis) {
		return glm::angleAxis(radians, axis);
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static bool IsPointInsideCircle(const Circle<T,Q>& circle, const glm::vec<2,T,Q>& point) {
		return Math::Distance(circle.Center, point) < circle.Radius;
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	[[nodiscard]] inline static bool IsPointInsideSphere(const Sphere<T,Q>& sphere, const glm::vec<3,T,Q>& point) {
		return Math::Distance(sphere.Center, point) < sphere.Radius;
	}
} // Imagine
