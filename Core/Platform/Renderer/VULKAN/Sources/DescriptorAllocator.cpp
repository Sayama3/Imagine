//
// Created by ianpo on 27/05/2025.
//

#include "Imagine/Vulkan/DescriptorAllocator.hpp"
#include "Imagine/Vulkan/VulkanMacros.hpp"

namespace Imagine::Vulkan {

	void DescriptorAllocator::InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios) {
		std::vector<VkDescriptorPoolSize> poolSizes;
		for (PoolSizeRatio ratio: poolRatios) {
			poolSizes.push_back(VkDescriptorPoolSize{
					.type = ratio.type,
					.descriptorCount = uint32_t(ratio.ratio * maxSets)});
		}

		VkDescriptorPoolCreateInfo pool_info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
		pool_info.flags = 0;
		pool_info.maxSets = maxSets;
		pool_info.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		pool_info.pPoolSizes = poolSizes.data();

		vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
	}

	void DescriptorAllocator::ClearDescriptors(VkDevice device) {
		vkResetDescriptorPool(device, pool, 0);
	}

	void DescriptorAllocator::DestroyPool(VkDevice device) {
		vkDestroyDescriptorPool(device, pool, nullptr);
	}


	VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout layout) {

		VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
		allocInfo.pNext = nullptr;
		allocInfo.descriptorPool = pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		VkDescriptorSet ds;
		VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &ds));

		return ds;
	}
} // namespace Imagine::Vulkan
