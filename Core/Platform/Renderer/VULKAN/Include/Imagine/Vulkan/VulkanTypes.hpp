//
// Created by ianpo on 01/06/2025.
//

#pragma once

#include <vk_mem_alloc.h>
#include "Imagine/Core/Math.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"

namespace Imagine::Vulkan {
	struct ComputePushConstants {
		glm::vec4 data1;
		glm::vec4 data2;
		glm::vec4 data3;
		glm::vec4 data4;
	};

	struct ComputeEffect {
		std::string name;
		VkPipeline pipeline;
		VkPipelineLayout layout;
		ComputePushConstants data;
	};

	struct AllocatedBuffer {
		VkBuffer buffer{nullptr};
		VmaAllocation allocation{nullptr};
		VmaAllocationInfo info{};
	};

	struct Vertex {
		glm::vec3 position;
		float uv_x;
		glm::vec3 normal;
		float uv_y;
		glm::vec4 color;
	};

	// holds the resources needed for a mesh
	struct GPUMeshBuffers {
		AllocatedBuffer indexBuffer;
		AllocatedBuffer vertexBuffer;
		VkDeviceAddress vertexBufferAddress;
	};

	// push constants for our mesh object draws
	struct GPUDrawPushConstants {
		glm::mat4 worldMatrix;
		VkDeviceAddress vertexBuffer;
	};


} // namespace Imagine::Vulkan
