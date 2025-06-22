//
// Created by ianpo on 27/05/2025.
//

#pragma once

#include <vk_mem_alloc.h>
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

	struct VulkanImGuiImage : public Core::ImGuiImage {
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
