//
// Created by Sayama on 23/05/2025.
//

#pragma once

#include "Imagine/Rendering/Renderer.hpp"

#include "Imagine/Vulkan/AllocatedImage.hpp"
#include "Imagine/Vulkan/DescriptorAllocator.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"
#include "Imagine/Vulkan/VulkanDeleter.hpp"
#include "Imagine/Vulkan/VulkanFrameData.hpp"
#include "Imagine/Vulkan/VulkanTypes.hpp"

namespace Imagine::Vulkan {
	class VulkanRenderer final : public Core::Renderer {
	public:
		explicit VulkanRenderer(const Core::ApplicationParameters &appParams);
		virtual ~VulkanRenderer() override;

	public:
		virtual void PrepareShutdown() override;

	private:
		void InitializeVulkan();
		void InitializeSwapChain();
		void InitializeCommands();
		void InitializeSyncStructures();
		void InitializeDescriptors();
		void InitializePipelines();

		void InitGradientPipeline();
		void InitSkyPipeline();
		void InitTrianglePipeline();

		void CreateSwapChain(uint32_t width, uint32_t height);
		void DestroySwapChain();

	private:
		void ShutdownVulkan();
		[[nodiscard]] VulkanFrameData &GetCurrentFrame();
		[[nodiscard]] const Core::RendererParameters &GetRenderParams() const;

	public:
		virtual void Draw() override;
		virtual void SendImGuiCommands() override;
		void DrawBackground(VkCommandBuffer cmd);
		void DrawGeometry(VkCommandBuffer cmd);

		void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)> &&function);
		void InitializeImGui();

	private:
		void DrawImGui(VkCommandBuffer cmd, VkImageView targetImageView);

	private:
		std::vector<ComputeEffect> m_BackgroundEffects;
		int m_CurrentBackgroundEffect{0};

	private:
		VkInstance m_Instance{nullptr}; // Vulkan library handle
		VkDebugUtilsMessengerEXT m_DebugMessenger{nullptr}; // Vulkan debug output handle
		VkPhysicalDevice m_ChosenGPU{nullptr}; // GPU chosen as the default device
		VkDevice m_Device{nullptr}; // Vulkan device for commands
		VkSurfaceKHR m_Surface{nullptr}; // Vulkan window surface

		VkSwapchainKHR m_Swapchain{nullptr};
		VkFormat m_SwapchainImageFormat;

		std::vector<VkImage> m_SwapchainImages{};
		std::vector<VkImageView> m_SwapchainImageViews{};
		VkExtent2D m_SwapchainExtent{};

		std::vector<VulkanFrameData> m_Frames{};
		uint32_t m_FrameIndex{0};
		VkQueue m_GraphicsQueue{nullptr};
		uint32_t m_GraphicsQueueFamily{0};

		VmaAllocator m_Allocator{nullptr};

		DescriptorAllocator m_GlobalDescriptorAllocator{};

		// Image onto which we'll draw each frame before sending it to the framebuffer.
		AllocatedImage m_DrawImage{};
		VkExtent2D m_DrawExtent{0, 0};
		VkDescriptorSet m_DrawImageDescriptors{nullptr};
		VkDescriptorSetLayout m_DrawImageDescriptorLayout{nullptr};


		// immediate submit structures
		VkFence m_ImmFence{nullptr};
		VkCommandBuffer m_ImmCommandBuffer{nullptr};
		VkCommandPool m_ImmCommandPool{nullptr};

		VkPipelineLayout m_TrianglePipelineLayout{nullptr};
		VkPipeline m_TrianglePipeline{nullptr};

		Deleter m_MainDeletionQueue;
		Core::ApplicationParameters m_AppParams;
	};
} // namespace Imagine::Vulkan
