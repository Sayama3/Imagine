//
// Created by ianpo on 04/07/2025.
//

#pragma once

namespace Imagine::Core {
	class GPUMesh {
	public:
		GPUMesh() = default;
		GPUMesh(const GPUMesh&) = delete;
		GPUMesh& operator=(const GPUMesh&) = delete;
		virtual ~GPUMesh() = default;
		virtual uint64_t GetID() = 0;
	};
}