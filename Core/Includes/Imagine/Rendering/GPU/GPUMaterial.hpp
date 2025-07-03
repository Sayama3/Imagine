//
// Created by ianpo on 04/07/2025.
//

#pragma once

#include "Imagine/Core/SmartPointers.hpp"

namespace Imagine::Core {
	class GpuMaterial {
	public:
		virtual ~GpuMaterial() = default;

		virtual uint64_t GetID() = 0;
	};
}