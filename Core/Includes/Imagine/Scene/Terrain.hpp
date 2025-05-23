//
// Created by ianpo on 20/05/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/HeapArray.hpp"
#include "Imagine/Core/RawHeapArray.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/BufferView.hpp"

namespace Imagine::Core {

	class Terrain {
	public:
		Terrain();
		~Terrain();
	private:
		HeapArray<float> m_Terrain;
	};

} // Imagine::Core
