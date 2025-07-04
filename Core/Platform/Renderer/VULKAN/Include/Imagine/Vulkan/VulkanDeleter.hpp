//
// Created by ianpo on 27/05/2025.
//

#pragma once

#include <vk_mem_alloc.h>
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Vulkan/Descriptors.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"


namespace Imagine::Vulkan {
	class Deleter;
	using DeleterPtr = Deleter*;
	using SharedDeleter = std::shared_ptr<Deleter>;

	class Deleter {
	public:
		template<typename VmaType>
		struct VmaObject {
			VmaAllocator allocator;
			VmaAllocation allocation;
			VmaType data;
		};
	public:
		using ShutdownFunction = std::function<void(void)>;
		using VmaImage = VmaObject<VkImage>;
		using VmaBuffer = VmaObject<VkBuffer>;
		using VkType = std::variant<VmaAllocator, VkFence, VkSemaphore, VkCommandPool, VmaImage, VmaBuffer, VkImageView, VkDescriptorSetLayout, DescriptorAllocator, VkPipeline, VkPipelineLayout, ShutdownFunction, VkDescriptorPool, DescriptorAllocatorGrowable, VkSampler, DeleterPtr, SharedDeleter>;
	public:
		Deleter() = default;
		~Deleter() = default;
		Deleter(const Deleter&) = delete;
		Deleter& operator=(const Deleter&) = delete;
		Deleter(Deleter&& o) noexcept;
		Deleter& operator=(Deleter&& o) noexcept;
		void swap(Deleter& o) noexcept;

	public:
		/**
		 * Function that will add an object in the list of object to destroy.
		 * Add them in the order you allocated them as the Deleter will destroy them in the reverse order.
		 * (So the same way a normal stack allocation works).
		 * @param data The type to add. Will be transformed into a VkType if handled.
		 */
		void push(VkType data) {
			m_ToDelete.push_back(std::move(data));
		}

		template<typename VmaType>
		void push(VmaAllocator allocator, VmaAllocation allocation, VmaType data) {
			m_ToDelete.push_back(VmaObject<VmaType>{allocator, allocation, data});
		}

		void flush(VkDevice device);

		std::deque<VkType> m_ToDelete;
	};

	class AutoDeleter;
	class AutoDeleter {
	public:
		using Shared = std::shared_ptr<AutoDeleter>;
		static Shared MakeShared();
	public:
		AutoDeleter();
		~AutoDeleter();

		AutoDeleter(const AutoDeleter&) = delete;
		AutoDeleter& operator=(const AutoDeleter&) = delete;

		AutoDeleter(AutoDeleter&& o) noexcept;
		AutoDeleter& operator=(AutoDeleter&& o) noexcept;
	public:
		void swap(AutoDeleter& o) noexcept;
		void push(Deleter::VkType data);
	public:
		template<typename VmaType>
		void push(VmaAllocator allocator, VmaAllocation allocation, VmaType data) {
			m_Deleter.push(Deleter::VmaObject<VmaType>{allocator, allocation, data});
		}
	private:
		Deleter m_Deleter;
	};
} // namespace Imagine::Vulkan
