//
// Created by ianpo on 02/08/2024.
//

#include "Imagine/Physics/PhysicsDebugRenderer.hpp"
#include "Imagine/Math/BoundingBox.hpp"
#include "Imagine/Math/Math.hpp"
#include "Imagine/Physics/PhysicsTypeHelpers.hpp"
#include "Imagine/Rendering/CPU/CPUMaterial.hpp"

#include <Jolt/Renderer/DebugRenderer.h>

#include "Imagine/Rendering/RenderObject.hpp"

namespace Imagine {
	void PhysicsDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) {
		MGN_PROFILE_FUNCTION();
		// TODO: Do something as that's way to slow
		LineObject line;
		line.points.push_back({{inFrom.GetX(), inFrom.GetY(), inFrom.GetZ()}});
		line.points.push_back({{inTo.GetX(), inTo.GetY(), inTo.GetZ()}});
		s_Lines.push_back(line);
	}

	std::tuple<Imagine::Vertex, Imagine::Vertex, Imagine::Vertex> CreateTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor) {
		Vec3 a{inV1.GetX(), inV1.GetY(), inV1.GetZ()}, b{inV2.GetX(), inV2.GetY(), inV2.GetZ()}, c{inV3.GetX(), inV3.GetY(), inV3.GetZ()};
		Vec4 color = Convert(inColor);
		Vec3 normal = Math::CalculateTriangleNormal(a, b, c);
		auto [tangent, bitangent] = Math::CalculateTangentAndBitangent(normal);
		return {{a, 0, normal, 0, Vec4(tangent, 0), Vec4(bitangent, 0), color},
				{b, 0, normal, 0, Vec4(tangent, 0), Vec4(bitangent, 0), color},
				{c, 0, normal, 0, Vec4(tangent, 0), Vec4(bitangent, 0), color}};
	}

	void PhysicsDebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) {
		MGN_PROFILE_FUNCTION();
		auto [a,b,c] = CreateTriangle(inV1, inV2, inV3, inColor);
		s_Vertices.push_back(a);
		s_Vertices.push_back(b);
		s_Vertices.push_back(c);
	}

	void PhysicsDebugRenderer::DrawText3D(JPH::RVec3Arg inPosition, const std::string_view &inString, JPH::ColorArg inColor, float inHeight) {
		// TODO: Implement 3D Text Rendering.
		// MGN_CORE_TRACE("[PhysicsDebugRender] (Pos: {}, {}, {}) - {}", inPosition.GetX(), inPosition.GetY(), inPosition.GetZ(), inString);
	}

} // namespace Imagine
