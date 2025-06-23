//
// Created by Sayama on 23/05/2025.
//

#pragma once

#include "Imagine/Rendering/Renderer.hpp"
#include "Imagine/Core/Size.hpp"
#include "Imagine/Scene/Scene.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"

#include "Imagine/Vulkan/Descriptors.hpp"
#include "Imagine/Vulkan/VulkanDeleter.hpp"
#include "Imagine/Vulkan/VulkanFrameData.hpp"
#include "Imagine/Vulkan/VulkanImage.hpp"
#include "Imagine/Vulkan/VulkanMaterial.hpp"
#include "Imagine/Vulkan/VulkanTypes.hpp"

namespace Imagine::Vulkan {
	class VulkanRenderer final : public Core::Renderer {
	public:
		explicit VulkanRenderer(const Core::ApplicationParameters &appParams);
		virtual ~VulkanRenderer() override;

	public:
		static VulkanRenderer *Get();

		virtual void PrepareShutdown() override;
		virtual Core::RendererAPI GetAPI() override { return VulkanRenderer::GetStaticAPI(); }
		static Core::RendererAPI GetStaticAPI() { return Core::RendererAPI::Vulkan; }

	public:

		virtual Mat4 GetViewMatrix() const override;
		virtual Mat4 GetProjectionMatrix() const override;
		virtual Mat4 GetViewProjectMatrix() const override;
		virtual Rect<> GetViewport() const override;

		virtual Vec3 GetWorldPoint(const Vec2 screenPoint) const override;

	public:
		VkDevice GetDevice();
		VkPhysicalDevice GetPhysicalDevice();

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
		void InitMeshPipeline();

		void CreateSwapChain(uint32_t width, uint32_t height);
		void DestroySwapChain();

		void ResizeSwapChain();

	private:
		AllocatedBuffer CreateBuffer(uint64_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		void DestroyBuffer(AllocatedBuffer &buffer);
		void InitDefaultData();

	public:
		AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
		AllocatedImage CreateImage(const void *data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
		void DestroyImage(const AllocatedImage &img);

	public:
		GPUMeshBuffers UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);
		void PushDeletion(Deleter::VkType data) {
			m_MainDeletionQueue.push(std::move(data));
		}

		template<typename VmaType>
		void PushDeletion(VmaAllocation allocation, VmaType data) {
			m_MainDeletionQueue.push(m_Allocator, allocation, data);
		}
	public:
		VkDescriptorSetLayout GetGPUSceneDescriptorLayout();
		VkFormat GetColorImageFormat() const;
		VkFormat GetDepthImageFormat() const;
		void CreateDefaultSamplers();

		std::shared_ptr<VulkanMaterialInstance> GetDefaultMaterial() {return m_DefaultMaterial;}

	private:
		void ShutdownVulkan();
		[[nodiscard]] VulkanFrameData &GetCurrentFrame();
		[[nodiscard]] const Core::RendererParameters &GetRenderParams() const;

		void UpdateCache();

	public:
		virtual bool BeginDraw() override;
		virtual void EndDraw() override;
		virtual void Present() override;
		virtual void Draw(const Core::DrawContext & ctx) override;

		virtual void Draw() override;
		virtual void SendImGuiCommands() override;

		virtual void LoadExternalModelInScene(const std::filesystem::path& path, Core::Scene*, Core::EntityID parent = Core::EntityID::NullID) override;
		virtual void LoadCPUMeshInScene(const Core::CPUMesh& path, Core::Scene*, Core::EntityID entity = Core::EntityID::NullID) override;

		void DrawBackground(VkCommandBuffer cmd);
		void DrawGeometry(VkCommandBuffer cmd);

		void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)> &&function);
		void InitializeImGui();

		virtual Core::DrawContext& GetDrawContext() override;

	private:
		void DrawImGui(VkCommandBuffer cmd, VkImageView targetImageView);

	private:
		std::vector<ComputeEffect> m_BackgroundEffects;
		int m_CurrentBackgroundEffect{0};

	private:
		VkInstance m_Instance{nullptr}; // Vulkan library handle
		VkDebugUtilsMessengerEXT m_DebugMessenger{nullptr}; // Vulkan debug output handle
		VkPhysicalDevice m_PhysicalDevice{nullptr}; // GPU chosen as the default device
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

		DescriptorAllocatorGrowable m_GlobalDescriptorAllocator{};

		// Image onto which we'll draw each frame before sending it to the framebuffer.
		AllocatedImage m_DrawImage{};
		AllocatedImage m_DepthImage{};
		VkExtent2D m_DrawExtent{0, 0};
		float m_RenderScale{1.0f};

		VkDescriptorSet m_ImGuiImageDescriptors{nullptr};
		VkDescriptorSetLayout m_ImGuiImageDescriptorLayout{nullptr};

		VkDescriptorSet m_DrawImageDescriptors{nullptr};
		VkDescriptorSetLayout m_DrawImageDescriptorLayout{nullptr};

		std::optional<Rect<>> m_ImGuiViewport;

		GPUSceneData m_SceneData;
		VkDescriptorSetLayout m_GpuSceneDataDescriptorLayout{nullptr};

		// immediate submit structures
		VkFence m_ImmFence{nullptr};
		VkCommandBuffer m_ImmCommandBuffer{nullptr};
		VkCommandPool m_ImmCommandPool{nullptr};

		VkPipelineLayout m_TrianglePipelineLayout{nullptr};
		VkPipeline m_TrianglePipeline{nullptr};

		VkPipelineLayout m_MeshPipelineLayout;
		VkPipeline m_MeshPipeline;

		AllocatedImage m_WhiteImage;
		AllocatedImage m_BlackImage;
		AllocatedImage m_GreyImage;
		AllocatedImage m_ErrorCheckerboardImage;

		VkSampler m_DefaultSamplerLinear{nullptr};
		VkSampler m_DefaultSamplerNearest{nullptr};

		std::shared_ptr<VulkanMaterialInstance> m_DefaultMaterial{};
		GLTFMetallicRoughness m_MetalRoughMaterial{};

		VkDescriptorSetLayout m_SingleImageDescriptorLayout{nullptr};

		Deleter m_MainDeletionQueue;
		Core::ApplicationParameters m_AppParams;

		bool m_ResizeRequested{false};

		Core::DrawContext m_MainDrawContext;

		Mat4 ViewMatrixCached;
		Mat4 ProjectionMatrixCached;
		Mat4 ViewProjectMatrixCached;
		Mat4 InvViewProjectMatrixCached;
	};
} // namespace Imagine::Vulkan
