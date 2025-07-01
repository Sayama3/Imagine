//
// Created by ianpo on 29/06/2025.
//

#pragma once

#include "Imagine/Layers/Layer.hpp"

namespace Imagine::Core {
	class WindowResizeEvent;
}
namespace Imagine::Core {

	class ImGuiEvent;

	class ImGuiLayer final : public Layer {
	public:
		ImGuiLayer() = default;
		virtual ~ImGuiLayer() override;
	public:
		virtual void OnEvent(Event &event) override;
	private:
		bool OnResize(WindowResizeEvent& e);
		bool OnImGui(ImGuiEvent& e);
		void RenderImGuiDockspace();
		void RenderImGuiMenuBar();
	};

} // namespace Imagine::Core
// Imagine
