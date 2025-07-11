//
// Created by Sayama on 23/05/2025.
//

#pragma once

#include "Imagine/Rendering/Renderer.hpp"

namespace Imagine::CPU {
	class CPURenderer final : public Renderer {
	public:
		CPURenderer();
		CPURenderer(const ApplicationParameters &appParams);
		virtual ~CPURenderer() override;

		virtual RendererAPI GetAPI() override {return CPURenderer::GetStaticAPI();}
		static RendererAPI GetStaticAPI() { return RendererAPI::CPU; }

	public:
		virtual void Draw() override;
		virtual void SendImGuiCommands() override;
		virtual void PrepareShutdown() override;
	};
} // namespace Imagine::CPU
