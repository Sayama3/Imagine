//
// Created by ianpo on 27/05/2025.
//

#include "Imagine/Vulkan/VulkanImage.hpp"
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>

#include "Imagine/Rendering/Renderer.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"

using namespace Imagine::Core;

namespace Imagine::Vulkan {
	VulkanTexture2D::VulkanTexture2D() {
	}

	VulkanTexture2D::~VulkanTexture2D() {
		dynamic_cast<VulkanRenderer*>(Renderer::Get())->PushFrameDeletion(image.allocation, image.image);
		dynamic_cast<VulkanRenderer*>(Renderer::Get())->PushFrameDeletion(image.imageView);
		image = {};
	}

	uint64_t VulkanTexture2D::GetID() {
		return reinterpret_cast<uint64_t>(image.imageView);
	}

	VulkanTexture3D::VulkanTexture3D() {
	}
	VulkanTexture3D::~VulkanTexture3D() {
		dynamic_cast<VulkanRenderer*>(Renderer::Get())->PushFrameDeletion(image.allocation, image.image);
		dynamic_cast<VulkanRenderer*>(Renderer::Get())->PushFrameDeletion(image.imageView);
		image = {};
	}
	uint64_t VulkanTexture3D::GetID() {
		return reinterpret_cast<uint64_t>(image.imageView);
	}

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
