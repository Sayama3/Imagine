//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/GPU/GPUMesh.hpp"

namespace Imagine::Core {

	//TODO: Move the rendering to ECS like

	struct Renderable {
		Renderable() = default;
		virtual ~Renderable() = default;
		Renderable(const AssetHandle handle) : cpuMesh(handle) {}

		AssetHandle cpuMesh{NULL_ASSET_HANDLE};
		Ref<GPUMesh> gpuMesh{nullptr};
	};

} // namespace Imagine::Core
