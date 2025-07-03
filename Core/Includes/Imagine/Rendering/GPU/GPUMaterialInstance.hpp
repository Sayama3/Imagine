//
// Created by ianpo on 04/07/2025.
//

#pragma once

namespace Imagine::Core {

	class GpuMaterialInstance {
	public:

	public:
		virtual ~GpuMaterial() = default;

		virtual uint64_t GetID() = 0;
	};
}