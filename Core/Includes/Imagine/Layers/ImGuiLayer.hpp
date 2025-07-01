//
// Created by ianpo on 29/06/2025.
//

#pragma once

#include "Imagine/Layers/Layer.hpp"

namespace Imagine::Core {

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
		bool m_BlockEvents = true;
	};

} // namespace Imagine::Core
// Imagine
