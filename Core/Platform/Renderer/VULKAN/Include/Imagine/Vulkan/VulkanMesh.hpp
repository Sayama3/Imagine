//
// Created by Iannis on 17/06/2025.
//

#pragma once
#include <vulkan/vulkan.h>
#include "Imagine/Rendering/Mesh.hpp"

namespace Imagine::Vulkan {
	struct VulkanMesh : public Core::Mesh {

		VulkanMesh() = default;
		virtual ~VulkanMesh() {};

		VkBuffer indexBuffer{nullptr};
		VkDeviceAddress vertexBufferAddress{0};
	};
}