//
// Created by ianpo on 27/05/2025.
//

#pragma once

#include "Imagine/Vulkan/Vulkan.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/TypeHelper.hpp"

namespace Imagine::Vulkan {
	struct Deleter {

		template<typename VmaType>
		struct VmaObject {
			VmaAllocator allocator;
			VmaAllocation allocation;
			VmaType data;
		};

		using VmaImage = VmaObject<VkImage>;

		using VkType = std::variant<VmaAllocator, VkFence, VkSemaphore, VkCommandPool, VmaImage, VkImageView>;

		void push_back(VkType data) {
			m_ToDelete.push_back(std::move(data));
		}

		void flush() {
			// reverse iterate the deletion queue to execute all the functions
			for (auto it = m_ToDelete.rbegin(); it != m_ToDelete.rend(); ++it) {
				VkType& type = *it;
				if (VmaAllocator* vmaAllocator = std::get_if<VmaAllocator>(&type)) {
					vmaDestroyAllocator(*vmaAllocator);
				} else if (VkFence* fence = std::get_if<VkFence>(&type)) {
					vkDestroyFence(m_Device,  *fence, nullptr);
				} else if (VkSemaphore* semaphore = std::get_if<VkSemaphore>(&type)) {
					vkDestroySemaphore(m_Device,  *semaphore, nullptr);
				} else if (VkCommandPool* vkCommandPool = std::get_if<VkCommandPool>(&type)) {
					vkDestroyCommandPool(m_Device, *vkCommandPool, nullptr);
				} else if (VkImageView* vmaImageView = std::get_if<VkImageView>(&type)) {
					vkDestroyImageView(m_Device, *vmaImageView, nullptr);
				} else if (VmaImage* vmaImage = std::get_if<VmaImage>(&type)) {
					vmaDestroyImage(vmaImage->allocator, vmaImage->data, vmaImage->allocation);
				} else { // std::monostate
					MGN_CORE_ERROR("[Vulkan] Type at index {} not handled.", type.index());
				}
			}

			m_ToDelete.clear();
		}

		std::deque<VkType> m_ToDelete;
		VkDevice m_Device{nullptr};
	};
}
