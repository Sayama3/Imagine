//
// Created by ianpo on 13/06/2025.
//

#include "Imagine/Vulkan/VulkanRenderObject.hpp"

using namespace Imagine::Core;

namespace Imagine::Vulkan {
	AutoDeleteMeshAsset *VulkanRenderObject::GetVulkanMesh() {
		return dynamic_cast<AutoDeleteMeshAsset *>(Core::RenderObject::mesh.get());
	}
} // namespace Imagine::Vulkan
