//
// Created by ianpo on 08/07/2025.
//

#include "Imagine/Rendering/CPU/CPUVirtualTexture.hpp"

namespace Imagine::Core {

	VirtualTexture2D::~VirtualTexture2D() = default;
	uint32_t VirtualTexture2D::GetWidth() {
		return width;
	}
	uint32_t VirtualTexture2D::GetHeight() {
		return height;
	}
	uint32_t VirtualTexture2D::GetDepth() {
		return 1;
	}
	uint32_t VirtualTexture2D::GetChannels() {
		return channels;
	}
	ImagePixelType VirtualTexture2D::GetPixelType() {
		return type;
	}
	TextureUsage VirtualTexture2D::GetUsage() {
		return usage;
	}

	VirtualTexture3D::~VirtualTexture3D() = default;

	uint32_t VirtualTexture3D::GetWidth() {
		return width;
	}
	uint32_t VirtualTexture3D::GetHeight() {
		return height;
	}
	uint32_t VirtualTexture3D::GetDepth() {
		return depth;
	}
	uint32_t VirtualTexture3D::GetChannels() {
		return channels;
	}
	ImagePixelType VirtualTexture3D::GetPixelType() {
		return type;
	}
	TextureUsage VirtualTexture3D::GetUsage() {
		return usage;
	}
} // namespace Imagine::Core
