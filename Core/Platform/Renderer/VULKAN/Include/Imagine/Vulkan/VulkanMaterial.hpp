//
// Created by ianpo on 13/06/2025.
//

#pragma once

#include <vulkan/vulkan.h>
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/GPU/GPUMaterialInstance.hpp"
#include "Imagine/Rendering/MaterialComponents.hpp"
#include "Imagine/Rendering/MeshParameters.hpp"
#include "Imagine/Vulkan/Descriptors.hpp"
#include "Imagine/Vulkan/VulkanImage.hpp"

namespace Imagine::Vulkan {

	class VulkanRenderer;

	struct VulkanMaterialPipeline {
		VkPipeline pipeline;
		VkPipelineLayout layout;
	};

	struct VulkanMaterialInstance : public Core::GPUMaterialInstance {
		VulkanMaterialInstance() = default;
		virtual ~VulkanMaterialInstance() = default;

		virtual uint64_t GetID() override;

		VulkanMaterialPipeline *pipeline;
		VkDescriptorSet materialSet;
		Core::MaterialPass passType;
	};

	struct GLTFMetallicRoughness {
		VulkanMaterialPipeline opaquePipeline;
		VulkanMaterialPipeline transparentPipeline;
		VkDescriptorSetLayout materialLayout;
		Core::Topology topology = Core::Topology::Triangle;

		// Our target for Uniform Buffers is a 256 bytes alignments
		struct alignas(256) MaterialConstants {
			glm::vec4 colorFactors;
			glm::vec4 metal_factor;
			glm::vec4 rough_factor;
		};

		struct MaterialResources {
			AllocatedImage colorImage;
			VkSampler colorSampler;
			AllocatedImage metalImage;
			VkSampler metalSampler;
			AllocatedImage roughImage;
			VkSampler roughSampler;
			AllocatedImage normalImage;
			VkSampler normalSampler;
			AllocatedImage aoImage;
			VkSampler aoSampler;
			VkBuffer dataBuffer;
			uint32_t dataBufferOffset;
		};

		DescriptorWriter writer;

		void BuildPipeline(VulkanRenderer* renderer);
		void ClearResources(VkDevice device);

		VulkanMaterialInstance WriteMaterial(VkDevice device, Core::MaterialPass pass, const MaterialResources &resources, DescriptorAllocatorGrowable &descriptorAllocator);
	};
};

