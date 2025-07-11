//
// Created by Sayama on 23/05/2025.
//

#pragma once

#include "Imagine/Core/Size.hpp"
#include "Imagine/Rendering/Renderer.hpp"
#include "Imagine/Scene/Scene.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"

#include "Imagine/Vulkan/Descriptors.hpp"
#include "Imagine/Vulkan/VulkanDeleter.hpp"
#include "Imagine/Vulkan/VulkanFrameData.hpp"
#include "Imagine/Vulkan/VulkanImage.hpp"
#include "Imagine/Vulkan/VulkanMaterial.hpp"
#include "Imagine/Vulkan/VulkanTypes.hpp"

namespace Imagine::Vulkan {
	class VulkanRenderer final : public Renderer {
	public:
		explicit VulkanRenderer(const ApplicationParameters &appParams);
		virtual ~VulkanRenderer() override;

	public:
		static VulkanRenderer *Get();

		virtual void PrepareShutdown() override;
		virtual RendererAPI GetAPI() override { return VulkanRenderer::GetStaticAPI(); }
		static RendererAPI GetStaticAPI() { return RendererAPI::Vulkan; }

	public:
		virtual Mat4 GetViewMatrix() const override;
		virtual Mat4 GetProjectionMatrix() const override;
		virtual Mat4 GetViewProjectMatrix() const override;
		virtual Rect<> GetViewport() const override;

		virtual Vec3 GetWorldPoint(const Vec2 screenPoint) const override;

	public:
		VkDevice GetDevice();
		VkPhysicalDevice GetPhysicalDevice();
		VmaAllocator GetAllocator();

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

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	private:
		AllocatedBuffer CreateBuffer(uint64_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		void DestroyBuffer(AllocatedBuffer &buffer);
		void InitDefaultData();

	public:
		AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
		AllocatedImage CreateImage(const void *data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
		void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
		void DestroyImage(const AllocatedImage &img);

	public:
		GPUMeshBuffers UploadMesh(ConstBufferView indices, ConstBufferView vertices);

	public:
		void PushDeletion(Deleter::VkType data) {
			m_MainDeletionQueue.push(std::move(data));
		}

		template<typename VmaType>
		void PushDeletion(VmaAllocation allocation, VmaType data) {
			m_MainDeletionQueue.push(m_Allocator, allocation, data);
		}

		void PushFrameDeletion(Deleter::VkType data) {
			if (m_IsDrawing) {
				PushCurrentFrameDeletion(std::move(data));
			} else {
				PushNextFrameDeletion(std::move(data));
			}
		}
		template<typename VmaType>
		void PushFrameDeletion(VmaAllocation allocation, VmaType data) {
			if (m_IsDrawing) {
				GetCurrentFrame().m_DeletionQueue.push(m_Allocator, allocation, data);
			} else {
				GetNextFrame().m_DeletionQueue.push(m_Allocator, allocation, data);
			}
		}


		void PushCurrentFrameDeletion(Deleter::VkType data) {
			GetCurrentFrame().m_DeletionQueue.push(std::move(data));
		}
		template<typename VmaType>
		void PushCurrentFrameDeletion(VmaAllocation allocation, VmaType data) {
			GetCurrentFrame().m_DeletionQueue.push(m_Allocator, allocation, data);
		}

		void PushNextFrameDeletion(Deleter::VkType data) {
			GetNextFrame().m_DeletionQueue.push(std::move(data));
		}
		template<typename VmaType>
		void PushNextFrameDeletion(VmaAllocation allocation, VmaType data) {
			GetNextFrame().m_DeletionQueue.push(m_Allocator, allocation, data);
		}

	public:
		VkDescriptorSetLayout GetGPUSceneDescriptorLayout();
		VkFormat GetColorImageFormat() const;
		VkFormat GetDepthImageFormat() const;
		void CreateDefaultSamplers();

		// std::shared_ptr<VulkanMaterialInstance> GetDefaultMeshMaterial() { return m_DefaultMeshMaterial; }
		// std::shared_ptr<VulkanMaterialInstance> GetDefaultLineMaterial() { return m_DefaultLineMaterial; }
		// std::shared_ptr<VulkanMaterialInstance> GetDefaultPointMaterial() {return m_DefaultPointMaterial;}

	private:
		void ShutdownVulkan();
		[[nodiscard]] VulkanFrameData &GetCurrentFrame();
		[[nodiscard]] VulkanFrameData &GetNextFrame();
		[[nodiscard]] const RendererParameters &GetRenderParams() const;

		void UpdateCache();

	public:
		virtual bool Resize() override;

		virtual bool BeginDraw(const Imagine::GPUSceneData& sceneData, const Imagine::GPULightData& lightData) override;
		virtual void EndDraw() override;
		virtual void Present() override;
		virtual void Draw(const DrawContext &ctx) override;

		virtual void Draw() override;
		virtual void SendImGuiCommands() override;

		virtual Ref<GPUMesh> LoadMesh(const CPUMesh &mesh) override;
		virtual Ref<GPUMaterial> LoadMaterial(const CPUMaterial &material) override;
		virtual Ref<GPUMaterialInstance> LoadMaterialInstance(const CPUMaterialInstance &instance) override;
		virtual Ref<GPUTexture2D> LoadTexture2D(const CPUTexture2D &tex2d) override;
		virtual Ref<GPUTexture3D> LoadTexture3D(const CPUTexture3D &tex3d) override;

		void DrawBackground(VkCommandBuffer cmd);

		void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)> &&function);
		void InitializeImGui();

		virtual DrawContext &GetDrawContext() override;

	public:
		// GLTFMetallicRoughness &GetGLTFMaterial() { return m_MetalRoughMaterial; }
		DescriptorAllocatorGrowable &GetDescriptorAllocatorGrowable() { return m_GlobalDescriptorAllocator; }
		inline AllocatedImage GetWhiteImage() {return m_WhiteImage;}
		inline AllocatedImage GetBlackImage() {return m_BlackImage;}
		inline AllocatedImage GetGreyImage() {return m_GreyImage;}
	private:
		void DrawImGui(VkCommandBuffer cmd, VkImageView targetImageView);

	private:
		std::vector<ComputeEffect> m_BackgroundEffects;
		int m_CurrentBackgroundEffect{0};
		bool m_IsDrawing{false};
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
		bool m_ViewportFocused = false;

		GPUSceneData m_SceneData;
		GPULightData m_LightData;
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

		Ref<VulkanMaterialInstance> m_OpaqueInstance;
		Ref<VulkanMaterialInstance> m_TransparentInstance;
		Ref<VulkanMaterialInstance> m_LineInstance;

		// std::shared_ptr<VulkanMaterialInstance> m_DefaultMeshMaterial{};
		// std::shared_ptr<VulkanMaterialInstance> m_DefaultLineMaterial{};
		// std::shared_ptr<VulkanMaterialInstance> m_DefaultPointMaterial{};

		// GLTFMetallicRoughness m_MetalRoughMaterial{};
		// GLTFMetallicRoughness m_LineMetalRoughMaterial{};
		// GLTFMetallicRoughness m_PointMetalRoughMaterial{};

		VkDescriptorSetLayout m_SingleImageDescriptorLayout{nullptr};

		Deleter m_MainDeletionQueue;
		ApplicationParameters m_AppParams;

		bool m_ResizeRequested{false};

		DrawContext m_MainDrawContext;

		Mat4 ViewMatrixCached;
		Mat4 ProjectionMatrixCached;
		Mat4 ViewProjectMatrixCached;
		Mat4 InvViewProjectMatrixCached;
	};
} // namespace Imagine::Vulkan
