//
// Created by ianpo on 22/06/2025.
//

#include "Imagine/Vulkan/VulkanDeleter.hpp"

namespace Imagine::Vulkan {

	void Deleter::flush(VkDevice device) {
		MGN_CORE_ASSERT(device, "Vulkan Device is not set in the Vulkan Deleter.");
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = m_ToDelete.rbegin(); it != m_ToDelete.rend(); ++it) {
			VkType &type = *it;
			if (VmaAllocator *vmaAllocator = std::get_if<VmaAllocator>(&type)) {
				vmaDestroyAllocator(*vmaAllocator);
			}
			else if (VkFence *fence = std::get_if<VkFence>(&type)) {
				vkDestroyFence(device, *fence, nullptr);
			}
			else if (VkSemaphore *semaphore = std::get_if<VkSemaphore>(&type)) {
				vkDestroySemaphore(device, *semaphore, nullptr);
			}
			else if (VkCommandPool *vkCommandPool = std::get_if<VkCommandPool>(&type)) {
				vkDestroyCommandPool(device, *vkCommandPool, nullptr);
			}
			else if (VkImageView *vmaImageView = std::get_if<VkImageView>(&type)) {
				vkDestroyImageView(device, *vmaImageView, nullptr);
			}
			else if (VmaImage *vmaImage = std::get_if<VmaImage>(&type)) {
				vmaDestroyImage(vmaImage->allocator, vmaImage->data, vmaImage->allocation);
			}
			else if (VmaBuffer *vmaImage = std::get_if<VmaBuffer>(&type)) {
				vmaDestroyBuffer(vmaImage->allocator, vmaImage->data, vmaImage->allocation);
			}
			else if (VkDescriptorSetLayout *descriptorSetLayout = std::get_if<VkDescriptorSetLayout>(&type)) {
				vkDestroyDescriptorSetLayout(device, *descriptorSetLayout, nullptr);
			}
			else if (DescriptorAllocator *descriptorAllocator = std::get_if<DescriptorAllocator>(&type)) {
				descriptorAllocator->DestroyPool(device);
			}
			else if (VkPipeline *pipeline = std::get_if<VkPipeline>(&type)) {
				vkDestroyPipeline(device, *pipeline, nullptr);
			}
			else if (VkPipelineLayout *pipelineLayout = std::get_if<VkPipelineLayout>(&type)) {
				vkDestroyPipelineLayout(device, *pipelineLayout, nullptr);
			}
			else if (VkDescriptorPool *descriptorPool = std::get_if<VkDescriptorPool>(&type)) {
				vkDestroyDescriptorPool(device, *descriptorPool, nullptr);
			}
			else if (VkSampler *sampler = std::get_if<VkSampler>(&type)) {
				vkDestroySampler(device, *sampler, nullptr);
			}
			else if (DescriptorAllocatorGrowable *descriptorAllocatorGrowable = std::get_if<DescriptorAllocatorGrowable>(&type)) {
				descriptorAllocatorGrowable->DestroyPools(device);
			}
			else if (GLTFMetallicRoughness *material = std::get_if<GLTFMetallicRoughness>(&type)) {
				material->ClearResources(device);
			}
			else if (VulkanImGuiImage *imguiImage = std::get_if<VulkanImGuiImage>(&type)) {
				VulkanImGuiImage::Remove(*imguiImage);
			}
			else if (ShutdownFunction *shutdown = std::get_if<ShutdownFunction>(&type)) {
				(*shutdown)();
			}
			else { // std::monostate
				MGN_CORE_ERROR("[Vulkan] Type at index {} not handled.", type.index());
			}
		}

		m_ToDelete.clear();
	}
} // namespace Imagine::Vulkan
