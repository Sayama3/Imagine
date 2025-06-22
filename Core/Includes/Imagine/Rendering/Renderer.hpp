//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "Imagine/Application/ApplicationParameters.hpp"
#include "Imagine/Rendering/DrawContext.hpp"
#include "Imagine/Rendering/RendererParameters.hpp"
#include "Imagine/Scene/Entity.hpp"

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

		virtual DrawContext& GetDrawContext() = 0;

		virtual void LoadExternalModelInScene(const std::filesystem::path& path, Scene*, EntityID parent = EntityID::NullID) = 0;
		//TODO: LoadInternalModelInScene

		virtual void SendImGuiCommands() = 0;
		virtual void PrepareShutdown() = 0;
	};
} // namespace Imagine::Core
