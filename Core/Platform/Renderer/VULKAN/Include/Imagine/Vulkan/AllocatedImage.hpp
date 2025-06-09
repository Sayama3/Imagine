//
// Created by ianpo on 27/05/2025.
//

#pragma once

#include "Imagine/Vulkan/Vulkan.hpp"
#include <vk_mem_alloc.h>

namespace Imagine::Vulkan {

	struct AllocatedImage {
		VkImage image{nullptr};
		VkImageView imageView{nullptr};
		VmaAllocation allocation{nullptr};
		VkExtent3D imageExtent{0, 0, 0};
		VkFormat imageFormat{VK_FORMAT_UNDEFINED};
	};

} // namespace Imagine::Vulkan
// Imagine
