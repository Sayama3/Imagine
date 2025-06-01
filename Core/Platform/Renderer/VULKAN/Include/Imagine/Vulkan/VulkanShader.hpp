//
// Created by ianpo on 01/06/2025.
//

#pragma once

#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"

namespace Imagine::Vulkan {

struct VulkanShader {
	VkPipeline pipeline;
	VkPipelineLayout layout;
	std::vector<VkPushConstantRange> pushConstants;
	Core::Buffer pushConstantsData;
};

} // Vulkan
// Imagine
