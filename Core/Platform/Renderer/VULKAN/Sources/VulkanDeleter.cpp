//
// Created by ianpo on 22/06/2025.
//

#include "Imagine/Vulkan/VulkanDeleter.hpp"
#include "Imagine/Rendering/Renderer.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"

using namespace Imagine::Core;

namespace Imagine::Vulkan {

	Deleter::Deleter(Deleter &&o) noexcept {
		std::swap(m_ToDelete, o.m_ToDelete);
	}
	Deleter &Deleter::operator=(Deleter &&o) noexcept {
		std::swap(m_ToDelete, o.m_ToDelete);
		return *this;
	}
	void Deleter::swap(Deleter &o) noexcept {
		std::swap(m_ToDelete, o.m_ToDelete);
	}

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
			else if (VkImageView *vkImageView = std::get_if<VkImageView>(&type)) {
				vkDestroyImageView(device, *vkImageView, nullptr);
			}
			else if (VmaImage *vmaImage = std::get_if<VmaImage>(&type)) {
				vmaDestroyImage(vmaImage->allocator, vmaImage->data, vmaImage->allocation);
			}
			else if (VmaBuffer *vmaBuffer = std::get_if<VmaBuffer>(&type)) {
				vmaDestroyBuffer(vmaBuffer->allocator, vmaBuffer->data, vmaBuffer->allocation);
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
			else if (DeleterPtr *sub_deleter = std::get_if<DeleterPtr>(&type)) {
				(*sub_deleter)->flush(device);
			}
			else if (SharedDeleter *shared_deleter = std::get_if<SharedDeleter>(&type)) {
				(*shared_deleter)->flush(device);
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

	AutoDeleter::Shared AutoDeleter::MakeShared() {
		return std::make_shared<AutoDeleter>();
	}
	AutoDeleter::AutoDeleter() = default;

	AutoDeleter::~AutoDeleter() {
		using namespace Core;
		Renderer* renderer = Core::Renderer::Get();
		MGN_CORE_ASSERT(renderer, "We need a renderer to autodelete");
		VulkanRenderer* vkRenderer = dynamic_cast<VulkanRenderer*>(renderer);
		MGN_CORE_ASSERT(vkRenderer, "We need a vulkan renderer to autodelete the vulkan auto deleter.");
		m_Deleter.flush(vkRenderer->GetDevice());
	}
	AutoDeleter::AutoDeleter(AutoDeleter &&o) noexcept {
		swap(o);
	}
	AutoDeleter &AutoDeleter::operator=(AutoDeleter &&o) noexcept {
		swap(o);
		return *this;
	}
	void AutoDeleter::swap(AutoDeleter &o) noexcept {
		m_Deleter.swap(o.m_Deleter);
	}
	void AutoDeleter::push(Deleter::VkType data) {
		m_Deleter.push(std::move(data));
	}
} // namespace Imagine::Vulkan
