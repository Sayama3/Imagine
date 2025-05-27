//
// Created by ianpo on 27/05/2025.
//

#pragma once

#include "Imagine/Vulkan/Vulkan.hpp"
#include "Imagine/Vulkan/VulkanDeleter.hpp"

namespace Imagine::Vulkan {

	struct VulkanFrameData {
		VkCommandPool m_CommandPool = {};
		VkCommandBuffer m_MainCommandBuffer = {};
		VkSemaphore m_SwapchainSemaphore = {}, m_RenderSemaphore = {};
		VkFence m_RenderFence = {};

		// TODO: Replace the deletion queue with a list of handle to delete through a std::variant of some of the Vulkan
		//  possible objects to delete.
		Deleter m_DeletionQueue = {};
	};
}
