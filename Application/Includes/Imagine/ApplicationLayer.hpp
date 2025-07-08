//
// Created by Iannis on 24/06/2025.
//

#pragma once

#include "Imagine/Core.hpp"
#include "Imagine/Application/Window.hpp"
#include "Imagine/Layers/Layer.hpp"
#include "Imagine/Math/ChaikinCurves.hpp"
#include "Imagine/Rendering/Renderer.hpp"
#include "Imagine/Math/MeshGraph3D.hpp"

namespace Imagine::Application {
	using namespace Imagine::Core;
	class ApplicationLayer final : public Core::Layer {
	public:
		ApplicationLayer();
		~ApplicationLayer() override;
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event &event) override;
	private:

		void OnUpdate(Core::TimeStep timeStep);
		void OnImGuiRender();
		void OnRender(Core::DrawContext &);
	private:
		void MoveMouse(Vec2 pos);
	public:
		bool ChangeModelAndPath(const std::filesystem::path& path);
	private:
		[[maybe_unused]] void ImGuiChaikin();
		[[maybe_unused]] void ImGuiSubdivide();

	private:
		Core::Window* m_Window{nullptr};
		Core::Renderer *m_Renderer{nullptr};


		Math::ChaikinCurves<> m_ChaikinCurves{};
		LineObject m_Line{};


		std::filesystem::path m_ModelPath{"Assets/Models/Box.glb"};
		bool m_MeshChanged = true;
		Ref<Core::CPUMesh> m_Mesh;
		Math::MeshGraph3D* m_MeshGraph{};
		Ref<Core::CPUMesh> m_SubdividedMesh;

		Core::EntityID m_OriginalMeshEntityID;
		Core::EntityID m_LoopMeshEntityID;

		Vec2 m_MousePos;
	};

} // namespace Imagine::Application
