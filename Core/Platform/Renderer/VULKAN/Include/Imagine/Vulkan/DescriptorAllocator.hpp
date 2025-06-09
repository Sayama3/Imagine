//
// Created by ianpo on 27/05/2025.
//

#pragma once

#include <vulkan/vulkan.h>

namespace Imagine::Vulkan {

	struct DescriptorAllocator {
		struct PoolSizeRatio {
			VkDescriptorType type;
			float ratio;
		};

		void InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
		void ClearDescriptors(VkDevice device);
		void DestroyPool(VkDevice device);

		VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout);

		VkDescriptorPool pool;
	};

} // namespace Imagine::Vulkan
