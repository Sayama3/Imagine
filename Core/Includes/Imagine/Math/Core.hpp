//
// Created by Iannis on 23/06/2025.
//

#pragma once

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS 1
#endif // GLM_FORCE_RADIANS

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 1
#endif // GLM_FORCE_DEPTH_ZERO_TO_ONE

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL 1
#endif // GLM_ENABLE_EXPERIMENTAL

// TODO: Figure out a way for a Left Handed with ; +X = Right ; +Y = Up ; +Z = Forward ;
#ifndef GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_LEFT_HANDED 1
#endif // GLM_FORCE_LEFT_HANDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numbers>

#include "Imagine/Core/Size.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/Profiling.hpp"

#ifdef MGN_DOUBLE

#define REAL_DECIMAL_DIG DBL_DECIMAL_DIG // # of decimal digits of rounding precision
#define REAL_DIG DBL_DIG // # of decimal digits of precision
#define REAL_EPSILON DBL_EPSILON // smallest such that 1.0+FLT_EPSILON != 1.0
#define REAL_HAS_SUBNORM DBL_HAS_SUBNORM // type does support subnormal numbers
#define REAL_GUARD FLT_GUARD
#define REAL_MANT_DIG DBL_MANT_DIG // # of bits in mantissa
#define REAL_MAX DBL_MAX // max value
#define REAL_MAX_10_EXP DBL_MAX_10_EXP // max decimal exponent
#define REAL_MAX_EXP DBL_MAX_EXP // max binary exponent
#define REAL_MIN DBL_MIN // min normalized positive value
#define REAL_MIN_10_EXP DBL_MIN_10_EXP // min decimal exponent
#define REAL_MIN_EXP DBL_MIN_EXP // min binary exponent
#define REAL_NORMALIZE FLT_NORMALIZE
#define REAL_RADIX DBL_RADIX // exponent radix
#define REAL_TRUE_MIN DBL_TRUE_MIN // min positive value

#else

#define REAL_DECIMAL_DIG FLT_DECIMAL_DIG // # of decimal digits of rounding precision
#define REAL_DIG FLT_DIG // # of decimal digits of precision
#define REAL_EPSILON FLT_EPSILON // smallest such that 1.0+FLT_EPSILON != 1.0
#define REAL_HAS_SUBNORM FLT_HAS_SUBNORM // type does support subnormal numbers
#define REAL_GUARD FLT_GUARD
#define REAL_MANT_DIG FLT_MANT_DIG // # of bits in mantissa
#define REAL_MAX FLT_MAX // max value
#define REAL_MAX_10_EXP FLT_MAX_10_EXP // max decimal exponent
#define REAL_MAX_EXP FLT_MAX_EXP // max binary exponent
#define REAL_MIN FLT_MIN // min normalized positive value
#define REAL_MIN_10_EXP FLT_MIN_10_EXP // min decimal exponent
#define REAL_MIN_EXP FLT_MIN_EXP // min binary exponent
#define REAL_NORMALIZE FLT_NORMALIZE
#define REAL_RADIX FLT_RADIX // exponent radix
#define REAL_TRUE_MIN FLT_TRUE_MIN // min positive value

#endif

#define MGN_SQR(a) (a * a)
#define GLM_VEC_T(L,T,Q) glm::vec<L,T,Q>

namespace Imagine {
	// Using Float for now. Adding 'typedef' to ease to transition if I ever want to go higher byte.
	// TODO: Search and replace the whole engine with this values.
#ifdef MGN_DOUBLE
	typedef double Real;
#else // MGN_FLOAT
	typedef float Real;
#endif

	typedef glm::qua<Real> Quat;
	typedef glm::vec<4, Real> Vec4;
	typedef glm::vec<3, Real> Vec3;
	typedef glm::vec<2, Real> Vec2;
	typedef glm::vec<1, Real> Vec1;

	typedef glm::mat<4, 4, Real> Mat4;
	typedef glm::mat<3, 3, Real> Mat3;
	typedef glm::mat<2, 2, Real> Mat2;
	typedef glm::mat<3, 2, Real> Mat3x2;


	namespace Math {

		inline static constexpr Vec3 Gravity = Vec3(0, -9.81, 0);
		inline static constexpr Real Epsilon = Real(1e-6);

		inline static Vec2 ToVec2(const Size2 s) {
			return {s.x, s.y};
		}
		inline static Vec3 ToVec3(const Size3 s) {
			return {s.x, s.y, s.z};
		}

		inline static void TRS(Mat4 &trs, const Vec3 &position, const glm::quat &rotation, const Vec3 &scale = Vec3(1.0)) {
			MGN_PROFILE_FUNCTION();
			trs = glm::translate(trs, position); // Translation Matrix
			trs = trs * glm::toMat4(rotation); // Rotation Matrix
			trs = glm::scale(trs, scale); // Scale Matrix
		}

		/**
		 * Create a Transform matrix from a position, a rotation, and a scale.
		 * @param position The position as a 3D vector.
		 * @param rotation The rotation as a quaternion.
		 * @param scale The scale as a 3D vector.
		 * @return A 4x4 matrix representation of the Transform matrix.
		 */
		inline static Mat4 TRS(const Vec3 &position, const glm::quat &rotation, const Vec3 &scale = Vec3(1.0)) {
			MGN_PROFILE_FUNCTION();
			Mat4 trs(1.0f);
			TRS(trs, position, rotation, scale);
			return trs; // Translation * Rotation * Scale => TRS Matrix.
		}

		inline static bool DecomposeTransform(const Mat4 &transform, Vec3 &position, Vec3 &rotation, Vec3 &scale) {
			MGN_PROFILE_FUNCTION();
			// from glm::decompose in matrix_decompose
			using namespace glm;
			using T = float;
			mat4 LocalMatrix(transform);

			// Normalize the matrix.
			if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
				return false;

			// First, isolate perspective.  This is the messiest.
			if (
					epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
					epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
					epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>())) {
				// Clear the perspective partition
				LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
				LocalMatrix[3][3] = static_cast<T>(1);
			}

			// Next take care of position (easy).
			position = vec3(LocalMatrix[3]);
			LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

			vec3 Row[3], Pdum3;

			// Now get scale and shear.
			for (length_t i = 0; i < 3; ++i)
				for (length_t j = 0; j < 3; ++j)
					Row[i][j] = LocalMatrix[i][j];

			// Compute X scale factor and normalize first row.
			scale.x = length(Row[0]);
			Row[0] = detail::scale(Row[0], static_cast<T>(1));
			scale.y = length(Row[1]);
			Row[1] = detail::scale(Row[1], static_cast<T>(1));
			scale.z = length(Row[2]);
			Row[2] = detail::scale(Row[2], static_cast<T>(1));

			// At this point, the matrix (in rows[]) is orthonormal.
			// Check for a coordinate system flip.  If the determinant
			// is -1, then negate the matrix and the scaling factors.
#if 0
			Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
			if (dot(Row[0], Pdum3) < 0)
			{
				for (length_t i = 0; i < 3; i++)
				{
					scale[i] *= static_cast<T>(-1);
					Row[i] *= static_cast<T>(-1);
				}
			}
#endif

			rotation.y = asin(-Row[0][2]);
			if (cos(rotation.y) != 0) {
				rotation.x = atan2(Row[1][2], Row[2][2]);
				rotation.z = atan2(Row[0][1], Row[0][0]);
			}
			else {
				rotation.x = atan2(-Row[2][0], Row[1][1]);
				rotation.z = 0;
			}

			return true;
		}

		inline static bool DecomposeTransform(const Mat4 &transform, Vec3 &position, Quat &rotation, Vec3 &scale) {
			MGN_PROFILE_FUNCTION();
			// from glm::decompose in matrix_decompose
			using namespace glm;
			using T = float;
			mat4 LocalMatrix(transform);

			// Normalize the matrix.
			if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
				return false;

			// First, isolate perspective.  This is the messiest.
			if (
					epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
					epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
					epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>())) {
				// Clear the perspective partition
				LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
				LocalMatrix[3][3] = static_cast<T>(1);
			}

			// Next take care of position (easy).
			position = vec3(LocalMatrix[3]);
			LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

			vec3 Row[3], Pdum3;

			// Now get scale and shear.
			for (length_t i = 0; i < 3; ++i)
				for (length_t j = 0; j < 3; ++j)
					Row[i][j] = LocalMatrix[i][j];

			// Compute X scale factor and normalize first row.
			scale.x = length(Row[0]);
			Row[0] = detail::scale(Row[0], static_cast<T>(1));
			scale.y = length(Row[1]);
			Row[1] = detail::scale(Row[1], static_cast<T>(1));
			scale.z = length(Row[2]);
			Row[2] = detail::scale(Row[2], static_cast<T>(1));

			// At this point, the matrix (in rows[]) is orthonormal.
			// Check for a coordinate system flip.  If the determinant
			// is -1, then negate the matrix and the scaling factors.
#if 0
			Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
			if (dot(Row[0], Pdum3) < 0)
			{
				for (length_t i = 0; i < 3; i++)
				{
					scale[i] *= static_cast<T>(-1);
					Row[i] *= static_cast<T>(-1);
				}
			}
#endif

			rotation.y = asin(-Row[0][2]);
			if (cos(rotation.y) != 0) {
				rotation.x = atan2(Row[1][2], Row[2][2]);
				rotation.z = atan2(Row[0][1], Row[0][0]);
			}
			else {
				rotation.x = atan2(-Row[2][0], Row[1][1]);
				rotation.z = 0;
			}

			return true;
		}

		template<typename T>
		inline static std::string to_string(const T &value) {
			MGN_PROFILE_FUNCTION();
			return glm::to_string(value);
		}

		template<typename T>
		inline static std::string ToString(const T &value) {
			MGN_PROFILE_FUNCTION();
			return glm::to_string(value);
		}

		template<typename T>
		inline constexpr static T Identity() {
			MGN_PROFILE_FUNCTION();
			return glm::identity<T>();
		}


		template<int C, int R, typename T, glm::qualifier Q = glm::defaultp>
		inline static const T *ValuePtr(const glm::mat<C, R, T, Q> &mat) {
			return glm::value_ptr(mat);
		}
		template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
		inline static const T *ValuePtr(const glm::vec<L, T, Q> &vec) {
			return glm::value_ptr(vec);
		}
		template<typename T, glm::qualifier Q = glm::defaultp>
		inline static const T *ValuePtr(const glm::qua<T, Q> &qua) {
			return glm::value_ptr(qua);
		}

		template<int C, int R, typename T, glm::qualifier Q = glm::defaultp>
		inline static T *ValuePtr(glm::mat<C, R, T, Q> &mat) {
			return glm::value_ptr(mat);
		}
		template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
		inline static T *ValuePtr(glm::vec<L, T, Q> &vec) {
			return glm::value_ptr(vec);
		}
		template<typename T, glm::qualifier Q = glm::defaultp>
		inline static T *ValuePtr(glm::qua<T, Q> &qua) {
			return glm::value_ptr(qua);
		}


		inline static constexpr Real Tau {6.28318530717958647692};
		inline static constexpr Real Pi  {3.14159265358979323846};
		inline static constexpr Real Phi {1.61803398874989484820};
		inline static constexpr Real DegToRad{Pi / static_cast<Real>(180)};
		inline static constexpr Real RadToDeg{static_cast<Real>(180) / Pi};

		inline static Real Cos(Real value) { return std::cos(value); };
		inline static Real Sin(Real value) { return std::sin(value); };

		template<typename T>
		inline static T Min(T v1, T v2) { return glm::min(v1, v2); }
		template<typename T>
		inline static T Max(T v1, T v2) { return glm::max(v1, v2); }
		template<typename T>
		inline static T Min(T v1, T v2, T v3) { return Min(Min(v1, v2), v3); }
		template<typename T>
		inline static T Max(T v1, T v2, T v3) { return Max(Max(v1, v2), v3); }
		template<typename T>
		inline static T Min(T v1, T v2, T v3, T v4) { return Min(Min(Min(v1, v2), v3), v4); }
		template<typename T>
		inline static T Max(T v1, T v2, T v3, T v4) { return Max(Max(Max(v1, v2), v3), v4); }
		template<typename T>
		inline static T Min(T v1, T v2, T v3, T v4, T v5) { return Min(Min(Min(Min(v1, v2), v3), v4), v5); }
		template<typename T>
		inline static T Max(T v1, T v2, T v3, T v4, T v5) { return Max(Max(Max(Max(v1, v2), v3), v4), v5); }
		template<typename T, typename TIter>
		inline static T Min(TIter begin, TIter end) { return std::max_element(begin, end, Min); }
		template<typename T, typename TIter>
		inline static T Max(TIter begin, TIter end) { return std::min_element(begin, end, Max); }
		template<typename T, typename TContainer>
		inline static T Min(const TContainer &container) { return Max(container.begin(), container.end()); }
		template<typename T, typename TContainer>
		inline static T Max(const TContainer &container) { return Min(container.begin(), container.end()); }

		/**
		 * @brief Clamps a value between a minimum and maximum bound.
		 *
		 * This function takes a value and clamps it between a minimum and maximum bound. The clamped value
		 * is the maximum value if it exceeds the maximum bound, the minimum value if it falls below the
		 * minimum bound, or the original value if it is within the bounds.
		 *
		 * @tparam T The type of the value to be clamped.
		 * @param value The value to be clamped.
		 * @param min The minimum bound.
		 * @param max The maximum bound.
		 * @return The clamped value.
		 *
		 * @note This function assumes that the `Max` and `Min` functions are available and correctly implement
		 * the comparison between the given values. It is recommended to check the implementation of `Max` and
		 * `Min` functions before using this clamp function.
		 *
		 * @see Max
		 * @see Min
		 */
		template<typename T>
		inline static T Clamp(T value, T min, T max) {
			return Math::Max(Math::Min(value, max), min);
		}


		/**
		 * @brief Returns the clamped value between 0 and 1.
		 *
		 * This template function clamps the given value between the minimum value of 0 and the maximum value of 1.
		 *
		 * @tparam T The type of the value.
		 * @param value The value to be clamped.
		 * @return The clamped value between 0 and 1.
		 *
		 * @note This function calls the Clamp function.
		 */
		template<typename T>
		inline static T Clamp01(T value) { return Clamp(value, T(0), T(1)); }

		inline static Mat4 ToMat4(const Quat &q) { MGN_PROFILE_FUNCTION(); return glm::toMat4(q); }
		inline static Mat3 ToMat3(const Quat &q) { MGN_PROFILE_FUNCTION(); return glm::toMat3(q); }

		inline static Mat4 Translate(const Mat4 &mat, Vec3 translation) { MGN_PROFILE_FUNCTION(); return glm::translate(mat, translation); }
		inline static Mat4 Scale(const Mat4 &mat, Vec3 scale) { MGN_PROFILE_FUNCTION(); return glm::scale(mat, scale); }

		inline static Real Pow(Real value, Real power) { MGN_PROFILE_FUNCTION(); return glm::pow(value, power); }

		/**
		 * Transforms a 3D point using a 4x4 transformation matrix.
		 *
		 * This function takes a 4x4 transformation matrix `mat` and a 3D point `point` as input.
		 * It transforms the point by multiplying it with the matrix, dividing by the w-component,
		 * and returns the transformed point in a 3D vector.
		 *
		 * @tparam T The type of the matrix and the point elements.
		 * @tparam Q The qualifier of the matrix and the point elements. It defaults to `glm::defaultp`.
		 * @param mat The 4x4 transformation matrix.
		 * @param point The 3D point to transform.
		 * @return The transformed point as a 3D vector.
		 *
		 */
		template<typename T, glm::qualifier Q = glm::defaultp>
		inline static glm::vec<3, T, Q> TransformPoint(const glm::mat<4, 4, T, Q> &mat, const glm::vec<3, T, Q> &point) {
			MGN_PROFILE_FUNCTION();
			auto vec = glm::vec<4, T, Q>(point, (T) 1);
			vec = mat * vec;
			vec /= vec.w;
			return vec;
		}

		/**
		 * Transforms a 3D direction using a 4x4 transformation matrix.
		 *
		 * This function takes a 4x4 transformation matrix `mat` and a 3D direction `point` as input.
		 * It transforms the direction by multiplying it with the matrix and returns the transformed direction
		 * in a 3D vector. The w-component of the resulting vector is set to 0.
		 *
		 * @tparam T The type of the matrix and the direction elements.
		 * @tparam Q The qualifier of the matrix and the direction elements. It defaults to `glm::defaultp`.
		 * @param mat The 4x4 transformation matrix.
		 * @param point The 3D direction to transform.
		 * @return The transformed direction as a 3D vector.
		 *
		 */
		template<typename T, glm::qualifier Q = glm::defaultp>
		inline static glm::vec<3, T, Q> TransformDirection(const glm::mat<4, 4, T, Q> &mat, const glm::vec<3, T, Q> &point) {
			MGN_PROFILE_FUNCTION();
			return mat * glm::vec<4, T, Q>(point, (T) 0);
		}

		/**
		 * Calculate the power of a value.
		 *
		 * This function calculates the power of a value to the given exponent.
		 * The function accepts a value of any type T and the exponent as an int64_t.
		 * It returns the result of the power operation.
		 *
		 * @tparam T The type of the value.
		 * @param value The value to calculate the power of.
		 * @param pow The exponent to raise the value to.
		 * @return The result of the power operation.
		 */
		template<typename T>
		inline static T Pow(const T &value, int64_t pow) {
			MGN_PROFILE_FUNCTION();
			T result = T(1);

			bool isNegative = pow < 0;
			auto power = isNegative ? -pow : pow;
			for (int i = 0; i < power; ++i) {
				result *= value;
			}

			return isNegative ? static_cast<T>(1) / result : result;
		}

		template<typename T>
		inline static constexpr T Pow2(const T &value) noexcept {
			return value * value;
		}

		inline static Real Sqrt(Real value) {
			MGN_PROFILE_FUNCTION();
			return glm::sqrt(value);
		}

		template<typename T, glm::qualifier Q = glm::defaultp>
		inline static glm::vec<3, T, Q> Cross(const glm::vec<3, T, Q> &a, const glm::vec<3, T, Q> &b) {
			return glm::cross(a, b);
		}

		template<typename T>
		inline static T Abs(const T &val) {
			return glm::abs(val);
		}

		inline static bool Approx(Real a, Real b, Real e = Epsilon) { return Abs(b - a) < e; }
		inline static bool Approx0(Real a, Real e = Epsilon) { return Abs(a) < e; }


		/**
		 * Calculates the dot product (also known as scalar product) of two vectors.
		 *
		 * Calculates the dot product of the two input vectors `vec1` and `vec2` and returns the result.
		 * This calculation is done using the `glm::dot` function.
		 *
		 * @tparam L The length of the vectors.
		 * @tparam T The type of the vector elements.
		 * @tparam Q The qualifier of the vector elements. It defaults to `glm::defaultp`.
		 * @param vec1 The first input vector.
		 * @param vec2 The second input vector.
		 * @return The dot product of the two input vectors.
		 *
		 * @see glm::dot
		 */
		template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
		static T Dot(const glm::vec<L, T, Q> &vec1, const glm::vec<L, T, Q> &vec2) {
			MGN_PROFILE_FUNCTION();
			return glm::dot(vec1, vec2);
		}

		/**
		 * Adds the given vector to this, scaled by the given amount.
		 * This is used to update the orientation quaternion by a rotation
		 * and time.
		 *
		 * @param vector The vector to add.
		 *
		 * @param scale The amount of the vector to add.
		 */
		template<typename T, glm::qualifier Q = glm::defaultp>
		inline static void AddScaledVector(glm::qua<T, Q> &rotation, const glm::vec<3, T, Q> &vector, T scale) {
			glm::qua<T, Q> q = glm::qua<T, Q>::wxyz(0, vector.x * scale, vector.y * scale, vector.z * scale);
			q *= rotation;
			rotation.w += q.w * ((T) 0.5);
			rotation.x += q.x * ((T) 0.5);
			rotation.y += q.y * ((T) 0.5);
			rotation.z += q.z * ((T) 0.5);
		}

		template<typename T, glm::qualifier Q = glm::defaultp>
		inline static glm::mat<3, 3, T, Q> SetSkewSymmetric(const glm::vec<3, T, Q> &vector) {
			return glm::mat<3, 3, T, Q>(
					0, -vector.z, +vector.y,
					+vector.z, 0, -vector.x,
					-vector.y, +vector.x, 0);
		}
	}; // namespace Math

	template<>
	inline std::string Math::ToString(const bool &value) { return std::to_string(value); }
	template<>
	inline std::string Math::ToString(const float &value) { return std::to_string(value); }
	template<>
	inline std::string Math::ToString(const double &value) { return std::to_string(value); }
	template<>
	inline std::string Math::ToString(const long double &value) { return std::to_string(value); }
	template<>
	inline std::string Math::ToString(const uint8_t &value) { return std::to_string(value); }
	template<>
	inline std::string Math::ToString(const uint16_t &value) { return std::to_string(value); }
	template<>
	inline std::string Math::ToString(const uint32_t &value) { return std::to_string(value); }
	template<>
	inline std::string Math::ToString(const uint64_t &value) { return std::to_string(value); }
	template<>
	inline std::string Math::ToString(const int8_t &value) { return std::to_string(value); }
	template<>
	inline std::string Math::ToString(const int16_t &value) { return std::to_string(value); }
	template<>
	inline std::string Math::ToString(const int32_t &value) { return std::to_string(value); }
	template<>
	inline std::string Math::ToString(const int64_t &value) { return std::to_string(value); }

	template<>
	inline float Math::Min(float value, float other) { return std::min(value, other); }
	template<>
	inline float Math::Max(float value, float other) { return std::max(value, other); }
	template<>
	inline double Math::Min(double value, double other) { return std::min(value, other); }
	template<>
	inline double Math::Max(double value, double other) { return std::max(value, other); }
	template<>
	inline long double Math::Min(long double value, long double other) { return std::min(value, other); }
	template<>
	inline long double Math::Max(long double value, long double other) { return std::max(value, other); }
	template<>
	inline uint8_t Math::Min(uint8_t value, uint8_t other) { return std::min(value, other); }
	template<>
	inline uint8_t Math::Max(uint8_t value, uint8_t other) { return std::max(value, other); }
	template<>
	inline uint16_t Math::Min(uint16_t value, uint16_t other) { return std::min(value, other); }
	template<>
	inline uint16_t Math::Max(uint16_t value, uint16_t other) { return std::max(value, other); }
	template<>
	inline uint32_t Math::Min(uint32_t value, uint32_t other) { return std::min(value, other); }
	template<>
	inline uint32_t Math::Max(uint32_t value, uint32_t other) { return std::max(value, other); }
	template<>
	inline uint64_t Math::Min(uint64_t value, uint64_t other) { return std::min(value, other); }
	template<>
	inline uint64_t Math::Max(uint64_t value, uint64_t other) { return std::max(value, other); }
	template<>
	inline int8_t Math::Min(int8_t value, int8_t other) { return std::min(value, other); }
	template<>
	inline int8_t Math::Max(int8_t value, int8_t other) { return std::max(value, other); }
	template<>
	inline int16_t Math::Min(int16_t value, int16_t other) { return std::min(value, other); }
	template<>
	inline int16_t Math::Max(int16_t value, int16_t other) { return std::max(value, other); }
	template<>
	inline int32_t Math::Min(int32_t value, int32_t other) { return std::min(value, other); }
	template<>
	inline int32_t Math::Max(int32_t value, int32_t other) { return std::max(value, other); }
	template<>
	inline int64_t Math::Min(int64_t value, int64_t other) { return std::min(value, other); }
	template<>
	inline int64_t Math::Max(int64_t value, int64_t other) { return std::max(value, other); }


} // namespace Imagine

namespace Imagine::Literal {
	constexpr Real operator""_r(const unsigned long long int value) { return static_cast<Real>(value); }
	constexpr Real operator""_r(const long double value) { return static_cast<Real>(value); }
} // namespace Imagine::Literal

template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline std::ostream &operator<<(std::ostream &os, const glm::mat<C, R, T, Q> &value) {
	MGN_PROFILE_FUNCTION();
	return os << glm::to_string(value);
}

template<typename T, glm::qualifier Q>
inline std::ostream &operator<<(std::ostream &os, const glm::qua<T, Q> &value) {
	MGN_PROFILE_FUNCTION();
	return os << glm::to_string(value);
}

template<glm::length_t L, typename T, glm::qualifier Q>
inline std::ostream &operator<<(std::ostream &os, const glm::vec<L, T, Q> &value) {
	MGN_PROFILE_FUNCTION();
	return os << glm::to_string(value);
}

template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
struct fmt::formatter<glm::mat<C, R, T, Q>> : fmt::formatter<std::string>
{
	auto format(const glm::mat<C, R, T, Q> &value, format_context &ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", glm::to_string(value));
	}
};

template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> : fmt::formatter<std::string>
{
	auto format(const glm::qua<T, Q> &value, format_context &ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", glm::to_string(value));
	}
};

template<glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L, T, Q>> : fmt::formatter<std::string>
{
	auto format(const glm::vec<L, T, Q> &value, format_context &ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", glm::to_string(value));
	}
};
