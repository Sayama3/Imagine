//
// Created by ianpo on 04/07/2025.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/GPU/GPUTexture3D.hpp"
#include "Imagine/Rendering/GPU/GPUTexture2D.hpp"

namespace Imagine {

	class CPUVirtualTexture2D final : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::Texture2D);
		Ref<GPUTexture2D> gpu{nullptr};
	};

	class CPUVirtualTexture3D final : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::Texture3D);
		Ref<GPUTexture3D> gpu{nullptr};
	};
}