//
// Created by ianpo on 01/06/2025.
//

#include "Imagine/Vulkan/VulkanShader.hpp"

namespace Imagine::Vulkan {
	const char *VulkanShader::GetName() const {
		return name.c_str();
	}
} // namespace Imagine::Vulkan
