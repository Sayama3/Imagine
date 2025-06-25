//
// Created by Iannis on 24/06/2025.
//

#pragma once

#include "Imagine/Application/Window.hpp"
#include "Imagine/Layers/Layer.hpp"
#include "Imagine/Math/ChaikinCurves.hpp"
#include "Imagine/Rendering/Renderer.hpp"

namespace Imagine::Application {
	using namespace Imagine::Core;
	class ApplicationLayer final : public Core::Layer {
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Core::TimeStep timeStep) override;
		virtual void OnImGuiRender() override;
		virtual void OnRender(Core::DrawContext &) override;
		virtual void OnEvent(Event &event) override;

	private:
		[[maybe_unused]] void ImGuiChaikin();
		[[maybe_unused]] void ImGuiLoop();
		[[maybe_unused]] void ImGuiKobbelt();

	private:
		Core::Window* m_Window{nullptr};
		Core::Renderer *m_Renderer{nullptr};

		Math::ChaikinCurves<> m_ChaikinCurves;
		LineObject m_Line;
		Core::CPUMesh m_Mesh;

		Core::EntityID m_CubeEntityID;
	};

} // namespace Imagine::Application
