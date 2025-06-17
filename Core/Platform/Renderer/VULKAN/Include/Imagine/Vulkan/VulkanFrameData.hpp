//
// Created by ianpo on 27/05/2025.
//

#pragma once

#include "Imagine/Vulkan/AllocatedImage.hpp"
#include "Imagine/Vulkan/Descriptors.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"
#include "Imagine/Vulkan/VulkanDeleter.hpp"

namespace Imagine::Vulkan {

	struct VulkanFrameData {
		VkCommandPool m_CommandPool = {};
		VkCommandBuffer m_MainCommandBuffer = {};

		VkSemaphore m_SwapchainSemaphore = {}, m_RenderSemaphore = {};
		VkFence m_RenderFence = {};
		uint32_t m_SwapchainImageIndex;

		// TODO: Add a Draw/Depth image per frame
		// AllocatedImage m_DrawImage{};
		// AllocatedImage m_DepthImage{};

		Deleter m_DeletionQueue = {};
		DescriptorAllocatorGrowable m_FrameDescriptors;
	};
} // namespace Imagine::Vulkan
