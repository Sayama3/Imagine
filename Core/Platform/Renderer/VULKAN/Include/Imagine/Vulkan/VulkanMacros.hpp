//
// Created by Iannis on 26/05/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"

#define VK_CHECK(vk_func)                                                                                                      \
	{                                                                                                                          \
		VkResult err = vk_func;                                                                                                \
		if (err) {                                                                                                             \
			const std::string vulkan_error_str = std::string{"[Vulkan] Detected error: "} + std::string{string_VkResult(err)}; \
			MGN_CORE_ERROR(vulkan_error_str);                                                                                  \
			throw std::runtime_error(vulkan_error_str);                                                                        \
		}                                                                                                                      \
	}

namespace Imagine::Vulkan {

}
