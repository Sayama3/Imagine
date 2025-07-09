//
// Created by ianpo on 01/06/2025.
//

#pragma once

#include <vk_mem_alloc.h>
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/CPU/CPUMesh.hpp"
#include "Imagine/Rendering/GPU/GPUMesh.hpp"
#include "Imagine/Rendering/MeshParameters.hpp"
#include "Imagine/Rendering/Light.hpp"
#include "Imagine/Rendering/GPU/GPULightData.hpp"
#include "Imagine/Rendering/GPU/GPUSceneData.hpp"
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
		glm::mat4 normalMatrix;
		VkDeviceAddress vertexBuffer;
	};

	struct GeoSurface {
		uint32_t startIndex{0};
		uint32_t count{0};
	};

	struct AutoDeleteMeshAsset final : public Core::GPUMesh {
		AutoDeleteMeshAsset();
		virtual ~AutoDeleteMeshAsset();
		AutoDeleteMeshAsset(const AutoDeleteMeshAsset&) = delete;
		AutoDeleteMeshAsset &operator=(const AutoDeleteMeshAsset &) = delete;

		virtual uint64_t GetID() override {return this->meshBuffers.vertexBufferAddress;}

		std::string name;
		std::vector<Core::LOD> lods{};
		GPUMeshBuffers meshBuffers;
	};

	struct ManualDeleteMeshAsset final : public Core::GPUMesh {
		ManualDeleteMeshAsset() = default;
		virtual ~ManualDeleteMeshAsset() = default;
		ManualDeleteMeshAsset(const ManualDeleteMeshAsset&) = delete;
		ManualDeleteMeshAsset& operator=(const ManualDeleteMeshAsset&) = delete;
		ManualDeleteMeshAsset(ManualDeleteMeshAsset&&) noexcept;
		ManualDeleteMeshAsset& operator=(ManualDeleteMeshAsset&&) noexcept;
		void swap(ManualDeleteMeshAsset& other) noexcept;

		virtual uint64_t GetID() override {return this->meshBuffers.vertexBufferAddress;}

		std::string name;
		std::vector<Core::LOD> lods{};
		GPUMeshBuffers meshBuffers;
	};


} // namespace Imagine::Vulkan
