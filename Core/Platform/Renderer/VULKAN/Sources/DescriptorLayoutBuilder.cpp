//
// Created by ianpo on 27/05/2025.
//

#include "Imagine/Vulkan/DescriptorLayoutBuilder.hpp"

namespace Imagine::Vulkan {
	void DescriptorLayoutBuilder::AddBinding(const uint32_t binding, const VkDescriptorType type) {
		VkDescriptorSetLayoutBinding newbind{};
		newbind.binding = binding;
		newbind.descriptorCount = 1;
		newbind.descriptorType = type;

		bindings.push_back(newbind);
	}

	void DescriptorLayoutBuilder::Clear() {
		bindings.clear();
	}

	VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device, VkShaderStageFlags shaderStages, void *pNext, VkDescriptorSetLayoutCreateFlags flags) {
		for (auto &b: bindings) {
			b.stageFlags |= shaderStages;
		}

		VkDescriptorSetLayoutCreateInfo info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
		info.pNext = pNext;

		info.pBindings = bindings.data();
		info.bindingCount = static_cast<uint32_t>(bindings.size());
		info.flags = flags;

		VkDescriptorSetLayout set;
		VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

		return set;
	}


} // namespace Imagine::Vulkan
