//
// Created by ianpo on 27/05/2025.
//

#include <vulkan/vulkan.h>
#include "Imagine/Vulkan/VulkanImage.hpp"
#include <imgui_impl_vulkan.h>

using namespace Imagine::Core;

namespace Imagine::Vulkan {
	VulkanImGuiImage VulkanImGuiImage::Add(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout, const uint32_t width, const uint32_t height) {
		VulkanImGuiImage img;
		img.size = {width, height};
#ifdef MGN_IMGUI
		img.descriptorSet = ImGui_ImplVulkan_AddTexture(sampler, imageView, imageLayout);
#else
		img.descriptorSet = nullptr;
#endif
		return img;
	}

	void VulkanImGuiImage::Remove(const VulkanImGuiImage &img) {
#ifdef MGN_IMGUI
		ImGui_ImplVulkan_RemoveTexture(img.descriptorSet);
#endif
	}

	uint64_t VulkanImGuiImage::GetImGuiID() {
		// Use C-Style cast to reproduce the ImGui examples.
		return (ImTextureID) descriptorSet;
	}

	glm::fvec2 VulkanImGuiImage::GetSize() {
		return size;
	}

} // namespace Imagine::Vulkan
