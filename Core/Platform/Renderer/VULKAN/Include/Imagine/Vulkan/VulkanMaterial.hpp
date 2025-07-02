//
// Created by ianpo on 13/06/2025.
//

#pragma once

#include <vulkan/vulkan.h>
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/Material.hpp"
#include "Imagine/Rendering/RenderTypes.hpp"
#include "Imagine/Vulkan/Descriptors.hpp"
#include "Imagine/Vulkan/VulkanImage.hpp"

namespace Imagine::Vulkan {

	class VulkanRenderer;

	struct VulkanMaterialPipeline {
		VkPipeline pipeline;
		VkPipelineLayout layout;
	};

	struct VulkanMaterialInstance : public Core::MaterialInstance {
		VulkanMaterialInstance() = default;
		virtual ~VulkanMaterialInstance() = default;

		VulkanMaterialPipeline *pipeline;
		VkDescriptorSet materialSet;
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

