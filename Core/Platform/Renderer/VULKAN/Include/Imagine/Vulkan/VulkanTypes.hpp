//
// Created by ianpo on 01/06/2025.
//

#pragma once

#include <vk_mem_alloc.h>
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/Mesh.hpp"
#include "Imagine/Rendering/Vertex.hpp"
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

	struct GeoSurface {
		uint32_t startIndex{0};
		uint32_t count{0};
	};

	struct MeshAsset final : public Core::Mesh {
		MeshAsset() = default;
		virtual ~MeshAsset();
		MeshAsset(const MeshAsset&) = delete;
		MeshAsset& operator=(const MeshAsset&) = delete;

		GPUMeshBuffers meshBuffers;
	};

	struct GPUSceneData {
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 viewproj;
		glm::vec4 ambientColor;
		glm::vec4 sunlightDirection; // w for sun power
		glm::vec4 sunlightColor;
	};


} // namespace Imagine::Vulkan
