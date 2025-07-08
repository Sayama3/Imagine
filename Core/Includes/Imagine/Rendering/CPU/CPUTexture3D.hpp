//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "Imagine/Core/InternalCore.hpp"
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Math/ImagePixelType.hpp"
#include "Imagine/Rendering/GPU/GPUTexture3D.hpp"

namespace Imagine::Core {

	class CPUTexture3D final : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::Texture3D);
	public:
		Ref<GPUTexture3D> gpu{nullptr};
		Buffer Buffer;
		uint32_t width, height, depth, channels;
		ImagePixelType pixelType;
	};
}