//
// Created by Sayama on 23/05/2025.
//

#pragma once

#include "Imagine/Rendering/Renderer.hpp"

namespace Imagine::CPU {
	class CPURenderer final : public Core::Renderer {
	public:
		CPURenderer();
		CPURenderer(const Core::ApplicationParameters &appParams);
		virtual ~CPURenderer() override;

		virtual Core::RendererAPI GetAPI() override {return CPURenderer::GetStaticAPI();}
		static Core::RendererAPI GetStaticAPI() { return Core::RendererAPI::CPU; }

	public:
		virtual void Draw() override;
		virtual void SendImGuiCommands() override;
		virtual void PrepareShutdown() override;
	};
} // namespace Imagine::CPU
