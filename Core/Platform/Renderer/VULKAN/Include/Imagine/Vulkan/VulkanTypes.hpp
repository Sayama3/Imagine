//
// Created by ianpo on 01/06/2025.
//

#pragma once

#include "Imagine/Core/Math.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"

namespace Imagine::Vulkan {
	struct ComputePushConstants {
		glm::vec4 data1;
		glm::vec4 data2;
		glm::vec4 data3;
		glm::vec4 data4;
	};

	struct ComputeEffect
	{
		std::string name;
		VkPipeline pipeline;
		VkPipelineLayout layout;
		ComputePushConstants data;
	};
}
