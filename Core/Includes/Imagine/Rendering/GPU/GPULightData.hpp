//
// Created by ianpo on 09/07/2025.
//

#pragma once

#include "Imagine/Math/Core.hpp"
#include "Imagine/Rendering/Light.hpp"
namespace Imagine::Core {
struct GPULightData {
	Imagine::Core::Light lights[32];
	int lightCount{0};
};
}
