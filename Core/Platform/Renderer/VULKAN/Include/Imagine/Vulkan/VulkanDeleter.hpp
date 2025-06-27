//
// Created by ianpo on 27/05/2025.
//

#pragma once

#include <vk_mem_alloc.h>
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/TypeHelper.hpp"
#include "Imagine/Vulkan/Descriptors.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"
#include "Imagine/Vulkan/VulkanMaterial.hpp"

namespace Imagine::Vulkan {
	struct Deleter {
		using ShutdownFunction = void (*)();

		template<typename VmaType>
		struct VmaObject {
			VmaAllocator allocator;
			VmaAllocation allocation;
			VmaType data;
		};

		using VmaImage = VmaObject<VkImage>;
		using VmaBuffer = VmaObject<VkBuffer>;

		using VkType = std::variant<VmaAllocator, VkFence, VkSemaphore, VkCommandPool, VmaImage, VmaBuffer, VkImageView, VkDescriptorSetLayout, DescriptorAllocator, VkPipeline, VkPipelineLayout, ShutdownFunction, VkDescriptorPool, DescriptorAllocatorGrowable, VkSampler, GLTFMetallicRoughness, VulkanImGuiImage>;


		/**
		 * Function that will add an object in the list of object to destroy.
		 * Add them in the order you allocated them as the Deleter will destroy them in the reverse order.
		 * (So the same way a normal stack allocation works).
		 * @param data The type to add. Will be transformed into a VkType if handled.
		 */
		void push(VkType data) {
			m_ToDelete.push_back(std::move(data));
		}

		template<typename VmaType>
		void push(VmaAllocator allocator, VmaAllocation allocation, VmaType data) {
			m_ToDelete.push_back(VmaObject<VmaType>{allocator, allocation, data});
		}

		void flush(VkDevice device);

		std::deque<VkType> m_ToDelete;
	};
} // namespace Imagine::Vulkan
