//
// Created by ianpo on 30/01/2025.
//

#pragma once

#include "Basics.hpp"
#include <stdexcept>

namespace Imagine::Math {

	template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	inline static GLM_VEC_T(3,T,Q) CalculateTriangleNormal(const GLM_VEC_T(3,T,Q)& a, const GLM_VEC_T(3,T,Q)& b, const GLM_VEC_T(3,T,Q)& c) {
		const GLM_VEC_T(3,T,Q) AB = b - a;
		const GLM_VEC_T(3,T,Q) AC = c - a;

		return Normalize(Cross(AB,AC));
	}
	template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	inline static std::pair<GLM_VEC_T(3,T,Q), GLM_VEC_T(3,T,Q)> CalculateTangentAndBitangent(const GLM_VEC_T(3,T,Q)& normal) {
		using vec3 = GLM_VEC_T(3,T,Q);
		vec3 tangent;
		vec3 bitangent;

		vec3 c1 = Cross(normal, vec3(0.0, 0.0, 1.0));
		vec3 c2 = Cross(normal, vec3(0.0, 1.0, 0.0));

		if (Magnitude(c1) > Magnitude(c2))
		{
			tangent = c1;
		}
		else
		{
			tangent = c2;
		}

		tangent = Normalize(tangent);

		bitangent = Cross(normal, tangent);
		bitangent = Normalize(bitangent);
		return {tangent, bitangent};
	}

	template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	inline static GLM_VEC_T(3,T,Q) CalculateTriangleNormalNonNormalized(const GLM_VEC_T(3,T,Q)& a, const GLM_VEC_T(3,T,Q)& b, const GLM_VEC_T(3,T,Q)& c) {
		const GLM_VEC_T(3,T,Q) AB = b - a;
		const GLM_VEC_T(3,T,Q) AC = c - a;

		return Cross(AB,AC);
	}

	// template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	// inline static bool IsTriangleOriented(const glm::vec<3,T,Q>& AB, const glm::vec<3,T,Q>& AC, const glm::vec<3,T,Q>& normal) {
	// 	return Dot(Cross(AB, AC), normal) > 0;
	// }

	template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	inline static bool IsTriangleOriented(const glm::vec<2,T,Q>& AB, const glm::vec<2,T,Q>& AC) {
		return Determinant(glm::mat<2,2,T,Q>{
			AB.x, AB.y,
			AC.x, AC.y,
		}) > 0;
	}

	// template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	// inline static bool IsTriangleOriented(const glm::vec<3,T,Q>& a, const glm::vec<3,T,Q>& b, const glm::vec<3,T,Q>& c, const glm::vec<3,T,Q>& normal) {
	// 	const auto AB = b - a;
	// 	const auto AC = c - a;
	// 	return IsTriangleOriented(AB, AC, normal);
	// }

	template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	inline static bool IsTriangleOriented(const glm::vec<2,T,Q>& a, const glm::vec<2,T,Q>& b, const glm::vec<2,T,Q>& c) {
		const auto AB = b - a;
		const auto AC = c - a;
		return IsTriangleOriented(AB, AC);
	}

	template<class fwd_iterator, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	inline static glm::vec<3,T,Q> CalculateSurfaceNormal(const fwd_iterator begin, const fwd_iterator end) {
		glm::vec<3,T,Q> normal{static_cast<T>(0)};
		for (auto it = begin; it != end; ++it) {
			glm::vec<3,T,Q> next;
			auto nextIt = it; ++nextIt;
			if (nextIt == end) next = *begin;
			else next = *nextIt;
			const glm::vec<3,T,Q> current = *it;

			normal.x += (next.y - current.y) * (next.z + current.z);
			normal.y += (next.z - current.z) * (next.x + current.x);
			normal.z += (next.x - current.x) * (next.y + current.y);
		}
		return Normalize(normal);
	}

	/**
	 * Calculate the signed angle in radians between the vector 1 & 2.
	 * @tparam T Type of the vector
	 * @tparam Q Alignment of the vector.
	 * @param x Vector 1.
	 * @param y Vector 2.
	 * @return Signed angle in radians between 0 & 2*PI.
	 */
	template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	inline static T SignedAngle(glm::vec<2,T,Q> x, glm::vec<2,T,Q> y) {
		const auto angle = Angle(x,y);
		if (IsTriangleOriented(x,y)) {
			return angle;
		} else {
			return static_cast<T>(2*Pi) - angle;
		}
	}

	// /**
	//  * Calculate the signed angle in radians between the vector 1 & 2.
	//  * @tparam T Type of the vector
	//  * @tparam Q Alignment of the vector.
	//  * @param x Vector 1.
	//  * @param y Vector 2.
	//  * @return Signed angle in radians between 0 & 2*PI.
	//  */
	// template<typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	// inline static T SignedAngle(glm::vec<3,T,Q> x, glm::vec<3,T,Q> y,  const glm::vec<3,T,Q>& normal) {
	// 	const auto angle = Angle(x,y);
	// 	if (IsTriangleOriented(x,y, normal)) {
	// 		return angle;
	// 	} else {
	// 		return static_cast<T>(2*Pi) - angle;
	// 	}
	// }

	template<class fwd_iterator, glm::length_t L, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	inline static glm::vec<L,T,Q> CalculateCenter(const fwd_iterator begin, const fwd_iterator end) {
		if (begin == end) return {};
		glm::vec<L,T,Q> center{static_cast<T>(0)};
		uint64_t count{0ull};
		for (auto it = begin; it != end; ++it) {
			center += *it;
			++count;
		}
		center /= static_cast<T>(count);
		return center;
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	inline static bool PointIsInsideTriangle(glm::vec<2,T,Q> a, glm::vec<2,T,Q> b, glm::vec<2,T,Q> c, glm::vec<2,T,Q> p) {
		if (a == b || a == c || b == c) return false;
		if (a == p || b == p || c == p) return true;

		{
			const auto AB = b - a;
			const auto AC = c - a;
			if (std::abs(Math::Dot(Math::Normalize(AB), Math::Normalize(AC))) >= 1 - REAL_EPSILON) return false;
		}

		if (!Math::IsTriangleOriented(a,b,c)) {
			std::swap(b,c);
		}

		return Math::IsTriangleOriented(a,b,p) && Math::IsTriangleOriented(b,c,p) && Math::IsTriangleOriented(c,a,p);
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	inline static glm::vec<3,T,Q> GetSphereCenter(const glm::vec<3,T,Q>& a, const glm::vec<3,T,Q>& b, const glm::vec<3,T,Q>& c) {
		const glm::vec<3,T,Q> cross_abbc = Math::Cross(a - b, b - c);
		const T divider = static_cast<T>(2) * Math::Magnitude2(cross_abbc);

		const T alpha = (Math::Magnitude2(b - c) * Math::Dot(a - b, a - c)) / divider;
		const T beta = (Math::Magnitude2(a - c) * Math::Dot(b - a, b - c)) / divider;
		const T gamma = (Math::Magnitude2(a - b) * Math::Dot(c - a, c - b)) / divider;

		return a * alpha + b * beta + c * gamma;
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	inline static T GetSphereRadius(const glm::vec<3,T,Q>& a, const glm::vec<3,T,Q>& b, const glm::vec<3,T,Q>& c) {
		const glm::vec<3,T,Q> cross_abbc = Math::Cross(a - b, b - c);
		const T divider = static_cast<T>(2) * Math::Magnitude(cross_abbc);
		const T radius = (Math::Magnitude(a - b) * Math::Magnitude(b - c) * Math::Magnitude(c - a)) / divider;
		return std::abs(radius);
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	inline static Sphere<T,Q> GetSphere(const glm::vec<3,T,Q>& a, const glm::vec<3,T,Q>& b, const glm::vec<3,T,Q>& c) {
		const glm::vec<3,T,Q> cross_abbc = Math::Cross(a - b, b - c);
		const T mag2_cross_abbc = Math::Magnitude2(cross_abbc);
		const T mag_cross_abbc = std::sqrt(mag2_cross_abbc);
		const T divider_rad = static_cast<T>(2) * std::sqrt(mag2_cross_abbc);
		const T divider_ct = static_cast<T>(2) * mag2_cross_abbc;

		const T alpha = (Math::Magnitude2(b - c) * Math::Dot(a - b, a - c)) / divider_ct;
		const T beta = (Math::Magnitude2(a - c) * Math::Dot(b - a, b - c)) / divider_ct;
		const T gamma = (Math::Magnitude2(a - b) * Math::Dot(c - a, c - b)) / divider_ct;

		const T radius = (Math::Magnitude(a-b) * Math::Magnitude(b-c) * Math::Magnitude(c-a)) / divider_rad;
		return Sphere{a * alpha + b * beta + c * gamma, std::abs(radius)};
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	inline static glm::vec<2,T,Q> GetCircleCenter(const glm::vec<2,T,Q>& a, const glm::vec<2,T,Q>& b, const glm::vec<2,T,Q>& c) {
		const auto ba = a - b;
		const auto cb = b - c;
		const auto ca = a - c;
		const T mag_cross_abbc = ba.x * cb.y - cb.x * ba.y; // Just the Z component of the Cross Product.
		const T divider_ct = static_cast<T>(2) * mag_cross_abbc * mag_cross_abbc;

		const T alpha = (Math::Magnitude2(cb) * Math::Dot(ba, ca)) / divider_ct;
		const T beta = (Math::Magnitude2(ca) * Math::Dot(b - a, cb)) / divider_ct;
		const T gamma = (Math::Magnitude2(ba) * Math::Dot(c - a, c - b)) / divider_ct;

		return (a * alpha) + (b * beta) + (c * gamma);
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	inline static T GetCircleRadius(const glm::vec<2,T,Q>& a, const glm::vec<2,T,Q>& b, const glm::vec<2,T,Q>& c) {
		const auto ba = a - b;
		const auto cb = b - c;
		const T mag_cross_abbc = ba.x * cb.y - cb.x * ba.y; // Just the Z component of the Cross Product.
		const T divider_rad = static_cast<T>(2) * mag_cross_abbc;

		const T radius = (Math::Magnitude(a-b) * Math::Magnitude(b-c) * Math::Magnitude(c-a)) / divider_rad;
		return std::abs(radius);
	}

	template<typename T, glm::qualifier Q = glm::qualifier::defaultp>
	inline static Circle<T,Q> GetCircle(const glm::vec<2,T,Q>& a, const glm::vec<2,T,Q>& b, const glm::vec<2,T,Q>& c) {
		const auto ba = a - b;
		const auto cb = b - c;
		const auto ca = a - c;
		const T mag_cross_abbc = ba.x * cb.y - cb.x * ba.y; // Just the Z component of the Cross Product.
		const T divider_rad = static_cast<T>(2) * mag_cross_abbc;
		const T divider_ct = static_cast<T>(2) * mag_cross_abbc * mag_cross_abbc;

		const T alpha = (Math::Magnitude2(cb) * Math::Dot(ba, ca)) / divider_ct;
		const T beta = (Math::Magnitude2(ca) * Math::Dot(b - a, cb)) / divider_ct;
		const T gamma = (Math::Magnitude2(ba) * Math::Dot(c - a, c - b)) / divider_ct;

		const T radius = (Math::Magnitude(ba) * Math::Magnitude(cb) * Math::Magnitude(ca)) / divider_rad;
		return Circle{a * alpha + b * beta + c * gamma, std::abs(radius)};
	}

}
