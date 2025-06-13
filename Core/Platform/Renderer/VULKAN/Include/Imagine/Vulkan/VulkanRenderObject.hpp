//
// Created by ianpo on 13/06/2025.
//

#pragma once

#include <vulkan/vulkan.h>
#include "Imagine/Vulkan/VulkanMaterial.hpp"
#include "Imagine/Core/Math.hpp"

namespace Imagine::Vulkan {

	struct VulkanRenderObject {
		uint32_t indexCount;
		uint32_t firstIndex;
		glm::mat4 transform;

		VkBuffer indexBuffer;
		VulkanMaterialInstance* material;
		VkDeviceAddress vertexBufferAddress;
	};


} // Vulkan
// Imagine
