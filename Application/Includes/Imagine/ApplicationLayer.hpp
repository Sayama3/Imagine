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

namespace Imagine::Runtime {

	using namespace Imagine;

	class ApplicationLayer final : public Layer {
	public:
		ApplicationLayer();
		~ApplicationLayer() override;
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event &event) override;
	private:

		void OnUpdate(TimeStep timeStep);
		void OnImGuiRender();
		void OnRender(DrawContext &);
	private:
		void MoveMouse(Vec2 pos);
	public:
		bool ChangeModelAndPath(const std::filesystem::path& path);
	private:
		[[maybe_unused]] void ImGuiChaikin();
		[[maybe_unused]] void ImGuiSubdivide();

	private:
		Window* m_Window{nullptr};
		Renderer *m_Renderer{nullptr};


		Math::ChaikinCurves<> m_ChaikinCurves{};
		LineObject m_Line{};


		std::filesystem::path m_ModelPath{"EngineAssets/Models/Box.glb"};
		bool m_MeshChanged = true;
		Ref<CPUMesh> m_Mesh;
		Math::MeshGraph3D* m_MeshGraph{};
		Ref<CPUMesh> m_SubdividedMesh;

		EntityID m_OriginalMeshEntityID;
		EntityID m_LoopMeshEntityID;

		Vec2 m_MousePos;
	};

} // namespace Imagine::Runtime
