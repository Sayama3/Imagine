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
		ImGuiLayer(bool blockEvents);
		virtual ~ImGuiLayer() override;

	public:
		void BlockEvents(bool block);
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event &event) override;
	private:
		bool OnResize(WindowResizeEvent& e);
		bool OnImGui(ImGuiEvent& e);
		void RenderImGuiDockspace();
		void RenderImGuiMenuBar();
	private:
		bool m_BlockEvents = true;
	};

} // namespace Imagine::Core
// Imagine
