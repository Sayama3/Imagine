//
// Created by ianpo on 27/05/2025.
//

#pragma once

#include <vk_mem_alloc.h>
#include "Imagine/Rendering/GPU/GPUTexture2D.hpp"
#include "Imagine/Rendering/GPU/GPUTexture3D.hpp"
#include "Imagine/Rendering/MgnImGui.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"

namespace Imagine::Vulkan {

	struct AllocatedImage {
		VkImage image{nullptr};
		VkImageView imageView{nullptr};
		VmaAllocation allocation{nullptr};
		VkExtent3D imageExtent{0, 0, 0};
		VkFormat imageFormat{VK_FORMAT_UNDEFINED};
	};

	struct VulkanTexture2D final : public GPUTexture2D {
		VulkanTexture2D();
		virtual ~VulkanTexture2D() override;
		VulkanTexture2D(const VulkanTexture2D&) = delete;
		VulkanTexture2D& operator=(const VulkanTexture2D&) = delete;

		virtual uint64_t GetID() override;

		AllocatedImage image;
		VkSampler sampler{nullptr};
	};

	struct VulkanTexture3D final : public GPUTexture3D {
		VulkanTexture3D();
		virtual ~VulkanTexture3D() override;
		VulkanTexture3D(const VulkanTexture3D&) = delete;
		VulkanTexture3D& operator=(const VulkanTexture3D&) = delete;

		virtual uint64_t GetID() override;

		AllocatedImage image;
		VkSampler sampler{nullptr};
	};

	struct VulkanImGuiImage : public ImGuiImage {
		virtual ~VulkanImGuiImage() = default;

		static VulkanImGuiImage Add(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout, uint32_t width, uint32_t height);
		static void Remove(const VulkanImGuiImage &img);

		virtual uint64_t GetImGuiID() override;
		virtual glm::fvec2 GetSize() override;

		VkDescriptorSet descriptorSet{nullptr};
		glm::fvec2 size{0};
	};

} // namespace Imagine::Vulkan
// Imagine
