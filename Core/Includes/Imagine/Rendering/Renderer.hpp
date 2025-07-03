//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "CPU/CPUMesh.hpp"
#include "Imagine/Application/ApplicationParameters.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/DrawContext.hpp"
#include "Imagine/Rendering/RendererParameters.hpp"
#include "Imagine/Scene/Entity.hpp"

#include "Imagine/Rendering/CPU/CPUMaterial.hpp"
#include "Imagine/Rendering/CPU/CPUMaterialInstance.hpp"
#include "Imagine/Rendering/CPU/CPUMesh.hpp"
#include "Imagine/Rendering/CPU/CPUShader.hpp"
#include "Imagine/Rendering/CPU/CPUTexture2D.hpp"
#include "Imagine/Rendering/CPU/CPUTexture3D.hpp"

#include "Imagine/Rendering/GPU/GPUMaterial.hpp"
#include "Imagine/Rendering/GPU/GPUMaterialInstance.hpp"
#include "Imagine/Rendering/GPU/GPUMesh.hpp"
#include "Imagine/Rendering/GPU/GPUTexture2D.hpp"
#include "Imagine/Rendering/GPU/GPUTexture3D.hpp"


namespace Imagine::Core {
	class Scene;

	enum class RendererAPI {
		CPU,
		Vulkan,
	};

	class Renderer {
	public:
		static Renderer *Create(ApplicationParameters appParams);
		static Renderer *Initialize(ApplicationParameters appParams);
		static void Shutdown();
		static Renderer *Get();

		static RendererAPI GetStaticAPI();
	private:
		static Renderer *s_Renderer;

	public:
		Renderer() = default;
		virtual ~Renderer() = default;
		Renderer(const Renderer &) = delete;
		Renderer &operator=(const Renderer &) = delete;

	public:
		virtual RendererAPI GetAPI() = 0;

		virtual bool BeginDraw() = 0;
		virtual void EndDraw() = 0;
		virtual void Present() = 0;
		virtual void Draw() = 0;
		virtual void Draw(const DrawContext& ctx) = 0;
		virtual bool Resize() = 0;

		virtual DrawContext& GetDrawContext() = 0;

		//TODO: Move this out
		virtual Mat4 GetViewMatrix() const = 0;
		virtual Mat4 GetProjectionMatrix() const = 0;
		virtual Mat4 GetViewProjectMatrix() const = 0;

		//TODO: Move this out.
		virtual Rect<> GetViewport() const = 0;

		//TODO: Move this out.
		virtual Vec3 GetWorldPoint(const Vec2 screenPoint) const = 0;

		virtual void LoadCPUMeshInScene(const CPUMesh& path, Scene*, EntityID entity = EntityID::NullID) = 0;

		virtual Ref<CPUMesh> LoadMesh(const CPUMesh& mesh) = 0;
		virtual Ref<GPUMaterial> LoadMaterial(const CPUMaterial& material) = 0;
		virtual Ref<GPUMaterialInstance> LoadMaterialInstance(const CPUMaterialInstance& instance) = 0;
		virtual Ref<GPUTexture2D> LoadTexture2D(const CPUTexture2D& tex2d) = 0;
		virtual Ref<GPUTexture3D> LoadTexture3D(const CPUTexture3D& tex3d) = 0;

		[[deprecated]] virtual void LoadExternalModelInScene(const std::filesystem::path& path, Scene*, EntityID parent = EntityID::NullID) = 0;

		virtual void SendImGuiCommands() = 0;
		virtual void PrepareShutdown() = 0;
	};
} // namespace Imagine::Core
