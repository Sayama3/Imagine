//
// Created by ianpo on 12/06/2025.
//

#include "Imagine/Vulkan/Descriptors.hpp"
#include "Imagine/Vulkan/VulkanMacros.hpp"

using namespace Imagine;

namespace Imagine::Vulkan {
	// =================================================================================
	// ============================== DescriptorAllocator ==============================
	// =================================================================================

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

	// =====================================================================================
	// ============================== DescriptorLayoutBuilder ==============================
	// =====================================================================================

	DescriptorLayoutBuilder& DescriptorLayoutBuilder::AddBinding(const uint32_t binding, const VkDescriptorType type) {
		VkDescriptorSetLayoutBinding newbind{};
		newbind.binding = binding;
		newbind.descriptorCount = 1;
		newbind.descriptorType = type;

		bindings.push_back(newbind);

		return *this;
	}

	void DescriptorLayoutBuilder::Clear() {
		bindings.clear();
	}

	VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device, VkShaderStageFlags shaderStages, void *pNext, VkDescriptorSetLayoutCreateFlags flags) {
		for (auto &b: bindings) {
			b.stageFlags |= shaderStages;
		}

		VkDescriptorSetLayoutCreateInfo info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
		info.pNext = pNext;

		info.pBindings = bindings.data();
		info.bindingCount = static_cast<uint32_t>(bindings.size());
		info.flags = flags;

		VkDescriptorSetLayout set;
		VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

		return set;
	}

	// =========================================================================================
	// ============================== DescriptorAllocatorGrowable ==============================
	// =========================================================================================
	void DescriptorAllocatorGrowable::Init(VkDevice device, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios) {
		ratios.clear();

		for (auto r: poolRatios) {
			ratios.push_back(r);
		}

		VkDescriptorPool newPool = CreatePool(device, initialSets, poolRatios);

		setsPerPool = initialSets * 1.5; // grow it next allocation

		readyPools.push_back(newPool);
	}

	void DescriptorAllocatorGrowable::ClearPools(VkDevice device) {
		for (auto p: readyPools) {
			vkResetDescriptorPool(device, p, 0);
		}
		for (auto p: fullPools) {
			vkResetDescriptorPool(device, p, 0);
			readyPools.push_back(p);
		}
		fullPools.clear();
	}

	void DescriptorAllocatorGrowable::DestroyPools(VkDevice device) {
		for (auto p: readyPools) {
			vkDestroyDescriptorPool(device, p, nullptr);
		}
		readyPools.clear();
		for (auto p: fullPools) {
			vkDestroyDescriptorPool(device, p, nullptr);
		}
		fullPools.clear();
	}

	VkDescriptorSet DescriptorAllocatorGrowable::Allocate(VkDevice device, VkDescriptorSetLayout layout, void *pNext) {
		// get or create a pool to allocate from
		VkDescriptorPool poolToUse = GetPool(device);

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.pNext = pNext;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = poolToUse;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		VkDescriptorSet ds;
		VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &ds);

		// allocation failed. Try again
		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {

			fullPools.push_back(poolToUse);

			poolToUse = GetPool(device);
			allocInfo.descriptorPool = poolToUse;

			VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &ds));
		}

		readyPools.push_back(poolToUse);
		return ds;
	}

	VkDescriptorPool DescriptorAllocatorGrowable::GetPool(VkDevice device) {

		VkDescriptorPool newPool;
		if (readyPools.size() != 0) {
			newPool = readyPools.back();
			readyPools.pop_back();
		}
		else {
			// need to create a new pool
			newPool = CreatePool(device, setsPerPool, ratios);

			setsPerPool = setsPerPool * 1.5;
			if (setsPerPool > c_MaxAmountSetsPerPool) {
				setsPerPool = c_MaxAmountSetsPerPool;
			}
		}

		return newPool;
	}

	VkDescriptorPool DescriptorAllocatorGrowable::CreatePool(VkDevice device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios) {
		std::vector<VkDescriptorPoolSize> poolSizes;
		for (PoolSizeRatio ratio: poolRatios) {
			poolSizes.push_back(VkDescriptorPoolSize{
					.type = ratio.type,
					.descriptorCount = uint32_t(ratio.ratio * setCount)});
		}

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = 0;
		pool_info.maxSets = setCount;
		pool_info.poolSizeCount = (uint32_t) poolSizes.size();
		pool_info.pPoolSizes = poolSizes.data();

		VkDescriptorPool newPool;
		vkCreateDescriptorPool(device, &pool_info, nullptr, &newPool);
		return newPool;
	}

	// ==============================================================================
	// ============================== DescriptorWriter ==============================
	// ==============================================================================

	void DescriptorWriter::WriteImage(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type) {
		VkDescriptorImageInfo &info = imageInfos.emplace_back(VkDescriptorImageInfo{
				.sampler = sampler,
				.imageView = image,
				.imageLayout = layout});

		VkWriteDescriptorSet write = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};

		write.dstBinding = binding;
		write.dstSet = VK_NULL_HANDLE; // left empty for now until we need to write it
		write.descriptorCount = 1;
		write.descriptorType = type;
		write.pImageInfo = &info;

		writes.push_back(write);
	}
	void DescriptorWriter::WriteBuffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type) {
		VkDescriptorBufferInfo &info = bufferInfos.emplace_back(VkDescriptorBufferInfo{
				.buffer = buffer,
				.offset = offset,
				.range = size});

		VkWriteDescriptorSet write = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
		write.dstBinding = binding;
		write.dstSet = VK_NULL_HANDLE; // left empty for now until we need to write it
		write.descriptorCount = 1;
		write.descriptorType = type;
		write.pBufferInfo = &info;

		writes.push_back(write);
	}

	void DescriptorWriter::Clear() {
		imageInfos.clear();
		bufferInfos.clear();
		writes.clear();
	}
	void DescriptorWriter::UpdateSet(VkDevice device, VkDescriptorSet set) {
		for (VkWriteDescriptorSet &write: writes) {
			write.dstSet = set;
		}

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
	}
} // namespace Imagine::Vulkan
