//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/GPU/GPUMesh.hpp"
#ifdef MGN_IMGUI
#include "Imagine/ThirdParty/ImGui.hpp"
#endif

namespace Imagine {

	//TODO: Move the rendering to ECS like

	struct Renderable {
		Renderable() = default;
		virtual ~Renderable() = default;
		Renderable(const AssetHandle handle) : cpuMeshOrModel(handle) {}

		AssetHandle cpuMeshOrModel{NULL_ASSET_HANDLE};
	};

	namespace ThirdParty::ImGuiLib {


		template<>
		inline bool RenderData<Renderable>(const char* label, Renderable* pRenderable) {
			bool changed = false;
#ifdef MGN_IMGUI
			changed |= ImGuiLib::DrawAssetField("Mesh Or Model", &pRenderable->cpuMeshOrModel, {AssetType::Model, AssetType::Mesh});
#endif
			return changed;
		}
	}
} // namespace Imagine
