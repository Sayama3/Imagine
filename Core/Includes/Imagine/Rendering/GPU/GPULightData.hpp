//
// Created by ianpo on 09/07/2025.
//

#pragma once

#include "Imagine/Math/Core.hpp"
#include "Imagine/Rendering/Light.hpp"
namespace Imagine {
struct GPULightData {
	static inline constexpr int MaxLight{32};
	Imagine::Light lights[MaxLight];
	int lightCount{0};
};
}
