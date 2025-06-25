//
// Created by Iannis on 24/06/2025.
//

#pragma once

#include "Imagine/Application/Window.hpp"
#include "Imagine/Layers/Layer.hpp"
#include "Imagine/Math/ChaikinCurves.hpp"
#include "Imagine/Math/MeshGraph3D.hpp"
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

	public:
		bool ChangeModelAndPath(const std::filesystem::path& path);
	private:
		[[maybe_unused]] void ImGuiChaikin();
		[[maybe_unused]] void ImGuiLoop();
		[[maybe_unused]] void ImGuiKobbelt();

	private:
		Core::Window* m_Window{nullptr};
		Core::Renderer *m_Renderer{nullptr};


		Math::ChaikinCurves<> m_ChaikinCurves;
		LineObject m_Line;


		std::filesystem::path m_ModelPath;
		bool m_MeshChanged = true;
		Core::CPUMesh m_Mesh;
		Math::MeshGraph3D<> m_MeshGraph;
		Core::CPUMesh m_SubdividedMesh;

		Core::EntityID m_OriginalMeshEntityID;
		Core::EntityID m_LoopMeshEntityID;
	};

} // namespace Imagine::Application
