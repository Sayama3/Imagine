//
// Created by ianpo on 29/06/2025.
//

#include "Imagine/Layers/ImGuiLayer.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#endif

namespace Imagine::Core {
	ImGuiLayer::ImGuiLayer(const bool blockEvents) : m_BlockEvents(blockEvents) {
	}
	ImGuiLayer::~ImGuiLayer() {

	}
#ifdef MGN_IMGUI

	void ImGuiLayer::BlockEvents(const bool block) {
		m_BlockEvents = block;
	}

	void ImGuiLayer::OnAttach() {
	}

	void ImGuiLayer::OnDetach() {
	}

	void ImGuiLayer::OnEvent(Event &e) {
		if(m_BlockEvents) {
			const ImGuiIO &io = ImGui::GetIO();
			e.m_Handled |= e.IsInCategory(EventCategory::EventCategoryMouse) & io.WantCaptureMouse;
			e.m_Handled |= e.IsInCategory(EventCategory::EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}
#else
	void ImGuiLayer::OnAttach() {}
	void ImGuiLayer::OnDetach() {}
	void ImGuiLayer::OnEvent(Event &event) {}
#endif
} // namespace Imagine::Core
