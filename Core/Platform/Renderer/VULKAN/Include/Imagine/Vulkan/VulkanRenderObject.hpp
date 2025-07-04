//
// Created by ianpo on 13/06/2025.
//

#pragma once

#include <vulkan/vulkan.h>
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/RenderObject.hpp"
#include "Imagine/Vulkan/VulkanMaterial.hpp"
#include "Imagine/Vulkan/VulkanTypes.hpp"

namespace Imagine::Vulkan {

	struct VulkanRenderObject final : public Core::RenderObject {
		VulkanRenderObject() = default;
		virtual ~VulkanRenderObject() = default;

		AutoDeleteMeshAsset* GetVulkanMesh();
	};


} // Vulkan
// Imagine
