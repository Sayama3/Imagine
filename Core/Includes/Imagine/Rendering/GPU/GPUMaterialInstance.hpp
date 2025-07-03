//
// Created by ianpo on 04/07/2025.
//

#pragma once

namespace Imagine::Core {

	class GPUMaterialInstance {
	public:

	public:
		GPUMaterialInstance() = default;
		virtual ~GPUMaterialInstance() = default;

		virtual uint64_t GetID() = 0;
	};
}