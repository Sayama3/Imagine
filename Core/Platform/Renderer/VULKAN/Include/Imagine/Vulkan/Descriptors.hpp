//
// Created by ianpo on 12/06/2025.
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

	// TODO: Add support for per-binding stage flags.
	struct DescriptorLayoutBuilder {
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		DescriptorLayoutBuilder& AddBinding(uint32_t binding, VkDescriptorType type);
		void Clear();
		VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shaderStages, void *pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
	};


	class DescriptorAllocatorGrowable {
	public:
		static inline constexpr uint32_t c_MaxAmountSetsPerPool{4092};
		struct PoolSizeRatio {
			VkDescriptorType type;
			float ratio;
		};

		void Init(VkDevice device, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
		void ClearPools(VkDevice device);
		void DestroyPools(VkDevice device);

		VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext = nullptr);
	private:
		VkDescriptorPool GetPool(VkDevice device);
		VkDescriptorPool CreatePool(VkDevice device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

		std::vector<PoolSizeRatio> ratios;
		std::vector<VkDescriptorPool> fullPools;
		std::vector<VkDescriptorPool> readyPools;
		uint32_t setsPerPool{0};
	};

	class DescriptorWriter {
	public:
		void WriteImage(int binding,VkImageView image,VkSampler sampler , VkImageLayout layout, VkDescriptorType type);
		void WriteBuffer(int binding,VkBuffer buffer,size_t size, size_t offset,VkDescriptorType type);

		void Clear();
		void UpdateSet(VkDevice device, VkDescriptorSet set);
	public:
		// std::deque is guaranteed to keep pointers to elements valid
		std::deque<VkDescriptorImageInfo> imageInfos;
		std::deque<VkDescriptorBufferInfo> bufferInfos;
		std::vector<VkWriteDescriptorSet> writes;
	};

}
