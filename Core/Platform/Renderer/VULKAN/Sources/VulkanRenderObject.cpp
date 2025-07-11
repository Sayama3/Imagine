//
// Created by ianpo on 13/06/2025.
//

#include "Imagine/Vulkan/VulkanRenderObject.hpp"

using namespace Imagine;

namespace Imagine::Vulkan {
	AutoDeleteMeshAsset *VulkanRenderObject::GetVulkanMesh() {
		return dynamic_cast<AutoDeleteMeshAsset *>(RenderObject::mesh.get());
	}
} // namespace Imagine::Vulkan
