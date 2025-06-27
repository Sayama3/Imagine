//
// Created by ianpo on 13/06/2025.
//

#include "Imagine/Vulkan/VulkanRenderObject.hpp"

namespace Imagine::Vulkan {
	MeshAsset *VulkanRenderObject::GetVulkanMesh() {
		return dynamic_cast<MeshAsset *>(Core::RenderObject::mesh.get());
	}
} // namespace Imagine::Vulkan
