//
// Created by ianpo on 08/07/2025.
//

#include "Imagine/Rendering/CPU/CPUVirtualTexture.hpp"

namespace Imagine::Core {

	CPUVirtualTexture2D::~CPUVirtualTexture2D() = default;
	uint32_t CPUVirtualTexture2D::GetWidth() {
		return width;
	}
	uint32_t CPUVirtualTexture2D::GetHeight() {
		return height;
	}
	uint32_t CPUVirtualTexture2D::GetDepth() {
		return 1;
	}
	uint32_t CPUVirtualTexture2D::GetChannels() {
		return channels;
	}
	ImagePixelType CPUVirtualTexture2D::GetPixelType() {
		return type;
	}
	TextureUsage CPUVirtualTexture2D::GetUsage() {
		return usage;
	}

	CPUVirtualTexture3D::~CPUVirtualTexture3D() = default;

	uint32_t CPUVirtualTexture3D::GetWidth() {
		return width;
	}
	uint32_t CPUVirtualTexture3D::GetHeight() {
		return height;
	}
	uint32_t CPUVirtualTexture3D::GetDepth() {
		return depth;
	}
	uint32_t CPUVirtualTexture3D::GetChannels() {
		return channels;
	}
	ImagePixelType CPUVirtualTexture3D::GetPixelType() {
		return type;
	}
	TextureUsage CPUVirtualTexture3D::GetUsage() {
		return usage;
	}
} // namespace Imagine::Core
