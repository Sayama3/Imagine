//
// Created by ianpo on 03/07/2025.
//

#pragma once
#include "Imagine/Core/InternalCore.hpp"
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Math/Image.hpp"
#include "Imagine/Rendering/GPU/GPUTexture2D.hpp"

namespace Imagine::Core {

	class CPUTexture2D final : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::Texture2D);
	public:
		CPUTexture2D() = default;
		virtual ~CPUTexture2D() override = default;
		CPUTexture2D(const Image<uint8_t>& i);
		CPUTexture2D(Image<uint8_t>&& i);

	public:
		Image<uint8_t> image;
		Ref<GPUTexture2D> gpu{nullptr};
	};

} // namespace Imagine::Core
