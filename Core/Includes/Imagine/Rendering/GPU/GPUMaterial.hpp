//
// Created by ianpo on 04/07/2025.
//

#pragma once

#include "Imagine/Core/SmartPointers.hpp"

namespace Imagine::Core {
	class GPUMaterial {
	public:
		GPUMaterial() = default;
		GPUMaterial(const GPUMaterial&) = delete;
		GPUMaterial& operator=(const GPUMaterial&) = delete;
		virtual ~GPUMaterial() = default;
		virtual uint64_t GetID() = 0;
	};
}