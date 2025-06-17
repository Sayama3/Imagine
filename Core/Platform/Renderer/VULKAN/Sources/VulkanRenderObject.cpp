//
// Created by ianpo on 13/06/2025.
//

#include "Imagine/Vulkan/VulkanRenderObject.hpp"

namespace Imagine::Vulkan {
	VulkanMesh *VulkanRenderObject::GetVulkanMesh() {
		return dynamic_cast<VulkanMesh *>(Core::RenderObject::mesh.get());
	}

	VulkanMaterialInstance *VulkanRenderObject::GetVulkanMaterialInstance() {
		return dynamic_cast<VulkanMaterialInstance *>(Core::RenderObject::material);
	}
} // namespace Imagine::Vulkan
