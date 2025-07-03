//
// Created by ianpo on 04/07/2025.
//

#pragma once

namespace Imagine::Core {
	class GPUTexture2D {
	public:
		GPUTexture2D() = default;
		GPUTexture2D(const GPUTexture2D&) = delete;
		GPUTexture2D& operator=(const GPUTexture2D&) = delete;
		virtual ~GPUTexture2D() = default;
		virtual uint64_t GetID() = 0;
	};
}