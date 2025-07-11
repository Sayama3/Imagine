//
// Created by ianpo on 04/07/2025.
//

#pragma once

namespace Imagine {
	class GPUTexture3D {
	public:
		GPUTexture3D() = default;
		GPUTexture3D(const GPUTexture3D&) = delete;
		GPUTexture3D& operator=(const GPUTexture3D&) = delete;
		virtual ~GPUTexture3D() = default;
		virtual uint64_t GetID() = 0;
	};
}