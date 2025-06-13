//
// Created by ianpo on 13/06/2025.
//

#pragma once

#include <vulkan/vulkan.h>

namespace Imagine::Vulkan {

	struct VulkanMaterialPipeline {
		VkPipeline pipeline;
		VkPipelineLayout layout;
	};

	struct VulkanMaterialInstance {
		VulkanMaterialPipeline* pipeline;
		VkDescriptorSet materialSet;
		uint64_t passType;
	};


} // Vulkan
// Imagine
