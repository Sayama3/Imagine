//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/GPU/GPUMesh.hpp"

namespace Imagine {

	//TODO: Move the rendering to ECS like

	struct Renderable {
		Renderable() = default;
		virtual ~Renderable() = default;
		Renderable(const AssetHandle handle) : cpuMeshOrModel(handle) {}

		AssetHandle cpuMeshOrModel{NULL_ASSET_HANDLE};
	};

} // namespace Imagine
