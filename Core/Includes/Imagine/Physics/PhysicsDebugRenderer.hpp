//
// Created by ianpo on 11/07/2025.
//

#pragma once
#include "Imagine/Assets/AssetHandle.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Math/Core.hpp"
#include "Imagine/Physics/PhysicsTypeHelpers.hpp"
#include "Imagine/Rendering/CPU/CPUMaterialInstance.hpp"
#include "Imagine/Rendering/CPU/CPUMesh.hpp"
#include "Imagine/Rendering/RenderObject.hpp"

namespace Imagine {

	class PhysicsDebugRenderer final : public JPH::DebugRendererSimple {
	public:
		inline static std::vector<Imagine::Vertex> s_Vertices{};
		inline static std::vector<LineObject> s_Lines{};
	public:
		virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
		virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
		virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view &inString, JPH::ColorArg inColor, float inHeight) override;
	};

} // namespace Imagine
