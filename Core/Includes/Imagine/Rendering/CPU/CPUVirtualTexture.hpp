//
// Created by ianpo on 08/07/2025.
//

#pragma once
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Math/ImagePixelType.hpp"
#include "Imagine/Rendering/GPU/GPUTexture2D.hpp"
#include "Imagine/Rendering/GPU/GPUTexture3D.hpp"
#include "Imagine/Rendering/TextureParameters.hpp"

namespace Imagine::Core {

	class CPUVirtualTexture : public Asset {
	public:
		virtual uint32_t GetWidth() = 0;
		virtual uint32_t GetHeight() = 0;
		virtual uint32_t GetDepth() = 0;
		virtual uint32_t GetChannels() = 0;
		virtual ImagePixelType GetPixelType() = 0;
		virtual TextureUsage GetUsage() = 0;
	public:
		TextureUsage usage;
		ImagePixelType type;
		uint32_t channels, width, height;
	};

	class CPUVirtualTexture2D final : public CPUVirtualTexture {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::Texture2D)
	public:
		~CPUVirtualTexture2D() override;
	public:
		virtual uint32_t GetWidth() override;
		virtual uint32_t GetHeight() override;
		virtual uint32_t GetDepth() override;
		virtual uint32_t GetChannels() override;
		virtual ImagePixelType GetPixelType() override;
		virtual TextureUsage GetUsage() override;
	public:
		Ref<GPUTexture2D> gpu;
	};

	class CPUVirtualTexture3D final : public CPUVirtualTexture {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::Texture3D);
	public:
		~CPUVirtualTexture3D() override;
	public:
		virtual uint32_t GetWidth() override;
		virtual uint32_t GetHeight() override;
		virtual uint32_t GetDepth() override;
		virtual uint32_t GetChannels() override;
		virtual ImagePixelType GetPixelType() override;
		virtual TextureUsage GetUsage() override;
	public:
		uint32_t depth;
		Ref<GPUTexture3D> gpu;
	};
}