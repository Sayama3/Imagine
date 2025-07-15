//
// Created by ianpo on 11/07/2025.
//

#pragma once

#include "Imagine/Math/Core.hpp"

namespace Imagine {

	template<typename T>
	struct Transform {
		using vec3 = glm::vec<3, T>;
		using vec4 = glm::vec<4, T>;
		using quat = glm::qua<T>;
		using mat4 = glm::mat<4,4,T>;
		using mat3 = glm::mat<3,3,T>;

		Transform() = default;
		~Transform() = default;
		Transform(const vec3& pos, const quat& rot, const vec3& scale, const mat4& parent) : LocalPosition(pos), LocalRotation(rot), LocalScale(scale) {
			PositionLocalToWorld = parent * Math::TRS(LocalPosition, LocalRotation, LocalScale);
			PositionWorldToLocal = glm::inverse(PositionLocalToWorld);
			NormalLocalToWorld = glm::transpose(PositionWorldToLocal);
		}

		vec3 GetWorldPosition() const {
			return PositionLocalToWorld[3];
		}
		vec3 GetWorldScale() const {
			vec3 size;
			size.x = glm::length(vec3(PositionLocalToWorld[0])); // Basis vector X
			size.y = glm::length(vec3(PositionLocalToWorld[1])); // Basis vector Y
			size.z = glm::length(vec3(PositionLocalToWorld[2])); // Basis vector Z
			return size;
		}

		vec3 GetWorldEulerRotation() const {
			const glm::vec3 left	= glm::normalize(glm::vec3(PositionLocalToWorld[0])); // Normalized left axis
			const glm::vec3 up	= glm::normalize(glm::vec3(PositionLocalToWorld[1])); // Normalized up axis
			const glm::vec3 forward	= glm::normalize(glm::vec3(PositionLocalToWorld[2])); // Normalized forward axis

			// Obtain the "unscaled" transform matrix
			glm::mat4 m(0.0f);
			m[0][0] = left.x;
			m[0][1] = left.y;
			m[0][2] = left.z;

			m[1][0] = up.x;
			m[1][1] = up.y;
			m[1][2] = up.z;

			m[2][0] = forward.x;
			m[2][1] = forward.y;
			m[2][2] = forward.z;

			glm::vec3 eulerRot;
			eulerRot.x = atan2f( m[1][2], m[2][2]);
			eulerRot.y = atan2f(-m[0][2], sqrtf(m[1][2] * m[1][2] + m[2][2] * m[2][2]));
			eulerRot.z = atan2f( m[0][1], m[0][0]);

			return eulerRot;
		}

		quat GetWorldRotation() const {
			return quat(GetWorldEulerRotation());
		}

		vec3 GetLocalPosition() const {
			return LocalPosition;
		}

		vec3 GetLocalScale() const {
			return LocalScale;
		}

		quat GetLocalRotation() const {
			return LocalRotation;
		}

		vec3 TransformPosition(vec3 pos) const {
			vec4 wpos = PositionLocalToWorld * vec4(pos,1);
			wpos /= wpos.w;
			return wpos;
		}

		vec3 TransformDirection(vec3 dir) const {
			return PositionLocalToWorld * vec4(dir,0);
		}

		vec3 InvTransformPosition(vec3 pos) const {
			vec4 wpos = PositionWorldToLocal * vec4(pos,1);
			wpos /= wpos.w;
			return wpos;
		}

		vec3 InvTransformDirection(vec3 dir) const {
			return PositionWorldToLocal * vec4(dir,0);
		}

		vec3 GetRight() const {
			return TransformDirection(vec3{1,0,0});
		}
		vec3 GetUp() const {
			return TransformDirection(vec3{0,1,0});
		}
		vec3 GetForward() const {
			return TransformDirection(vec3{0,0,1});
		}

		vec3 LocalPosition{0};
		quat LocalRotation{Math::Identity<quat>()};
		vec3 LocalScale{1};
		mat4 PositionLocalToWorld{Math::Identity<mat4>()};
		mat4 PositionWorldToLocal{Math::Identity<mat4>()};
		mat3 NormalLocalToWorld{Math::Identity<mat3>()};
	};

	using TransformR = Transform<Real>;
	using TransformF = Transform<float>;
	using TransformD = Transform<double>;
}