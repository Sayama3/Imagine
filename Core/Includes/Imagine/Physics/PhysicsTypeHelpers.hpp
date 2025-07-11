//
// Created by ianpo on 02/08/2024.
//

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include "Imagine/Core/Math.hpp"
#include "Imagine/Math/BoundingBox.hpp"
#include "Imagine/Math/Geometry.hpp"
#include "Imagine/Rendering/MeshParameters.hpp"

namespace Imagine
{
	template<typename T, typename JT>
	inline static constexpr T ConvertT(const JT val) {return *(T*)&val;}
	inline static constexpr Vec3 Convert(const JPH::Vec3 val) {return {val.GetX(),val.GetY(),val.GetZ()};}
	inline static constexpr Vec4 Convert(const JPH::Vec4 val) {return {val.GetX(),val.GetY(),val.GetZ(),val.GetW()};}
	inline static constexpr Vec4 Convert(const JPH::Color val) {return {Real(val.r) / Real(255),Real(val.g) / Real(255),Real(val.b) / Real(255),Real(val.a) / Real(255)};}
	inline static constexpr Quat Convert(const JPH::Quat val) {return Quat::wxyz(val.GetW(), val.GetX(), val.GetY(), val.GetZ());}
	inline static constexpr Mat4 Convert(const JPH::Mat44 &inModelMatrix) {return {
				inModelMatrix(0,0),inModelMatrix(1,0),inModelMatrix(2,0),inModelMatrix(3,0),
				inModelMatrix(0,1),inModelMatrix(1,1),inModelMatrix(2,1),inModelMatrix(3,1),
				inModelMatrix(0,2),inModelMatrix(1,2),inModelMatrix(2,2),inModelMatrix(3,2),
				inModelMatrix(0,3),inModelMatrix(1,3),inModelMatrix(2,3),inModelMatrix(3,3)
		};}
	inline static Vertex Convert(const JPH::DebugRenderer::Vertex &vert) {
		auto [tangent, bitangent] = Math::CalculateTangentAndBitangent({vert.mNormal.x,vert.mNormal.y,vert.mNormal.z});
		return {
			{vert.mPosition.x,vert.mPosition.y,vert.mPosition.z},
			vert.mUV.x,
			{vert.mNormal.x,vert.mNormal.y,vert.mNormal.z},
			vert.mUV.y,
			{tangent, 0},
			{bitangent, 0},
			Convert(vert.mColor)
		};
	}

	inline static constexpr BoundingBox Convert(const JPH::AABox &aabb) {
		return {
			Convert(aabb.mMin),
			Convert(aabb.mMax)
		};
	}

	inline static JPH::Vec3 Convert(const Vec3 val) {return {val.x,val.y,val.z};}
	inline static JPH::Vec4 Convert(const Vec4 val) {return {val.x,val.y,val.z,val.w};}
	inline static JPH::Quat Convert(const Quat val) {return {val.x, val.y, val.z, val.w};}

}
