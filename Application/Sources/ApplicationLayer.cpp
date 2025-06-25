//
// Created by Iannis on 24/06/2025.
//

#include "Imagine/ApplicationLayer.hpp"

#include "Imagine/Core/Inputs.hpp"
#include "Imagine/Rendering/Camera.hpp"
#include "Imagine/Scene/SceneManager.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#endif

using namespace Imagine::Core;

namespace Imagine::Application {

	void ApplicationLayer::OnAttach() {
		m_Window = Window::Get();
		m_Renderer = Renderer::Get();


		m_Mesh = CPUMesh::LoadExternalModelAsMesh("Assets/Models/Box.glb");
		m_CubeEntityID = SceneManager::GetMainScene()->CreateEntity();
		SceneManager::GetMainScene()->GetEntity(m_CubeEntityID).LocalPosition = {0, 0, 0};
		m_Renderer->LoadCPUMeshInScene(m_Mesh, SceneManager::GetMainScene().get(), m_CubeEntityID);

		m_Renderer->LoadExternalModelInScene("Assets/Models/house.glb", SceneManager::GetMainScene().get());

		m_ChaikinCurves.SetUV(0.3, 0.3);
	}

	void ApplicationLayer::OnDetach() {
	}

	void ApplicationLayer::OnUpdate(Core::TimeStep timeStep) {
		auto &mouse = Inputs::GetMouse();
		const Rect<> window = m_Window->GetWindowRect();
		const Rect<> viewport = m_Renderer->GetViewport();
		const Vec2 globalPos = mouse.GetPosition() + window.min;
		const Vec2 viewportPos = globalPos - viewport.min;


		if (mouse.IsButtonPressed(Mouse::Right)) {
			const Vec3 camPos = Camera::s_MainCamera->position;
			const Vec3 worldPos = m_Renderer->GetWorldPoint(viewportPos);
			const Vec3 fwd = Camera::s_MainCamera->GetForward();
			const auto ray = Ray3{camPos, Math::Normalize(worldPos - camPos)};

			const std::optional<Vec3> mouseOnGround = Math::RaycastToPoint(Plane{Vec3{0, 0, 0}, Vec3{0, 1, 0}}, ray);
			if (mouseOnGround && SceneManager::GetMainScene()->Exist(m_CubeEntityID)) {
				m_ChaikinCurves.AddPoint(mouseOnGround.value());
				// SceneManager::GetMainScene()->GetEntity(m_CubeEntityID).LocalPosition = mouseOnGround.value();
			}
		}
	}

	void ApplicationLayer::OnImGuiRender() {
		ImGuiChaikin();

		ImGuiLoop();

		ImGuiKobbelt();
	}

	void ApplicationLayer::OnRender(Core::DrawContext &ctx) {
		LineObject line;
		if (!m_ChaikinCurves.line.empty()) {
			line.points.clear();
			line.points.reserve(m_ChaikinCurves.line.size() + 1);
			for (const auto &p: m_ChaikinCurves.line) {
				Vertex v{};
				v.position = p;
				v.color = {0.2, 0.3, 0.8, 1.0};
				line.points.push_back(v);
			}
			line.points.push_back(line.points.front());
		}

		if (m_Line.points.size() > 1) ctx.OpaqueLines.push_back(m_Line);
		if (line.points.size() > 1) ctx.OpaqueLines.push_back(line);
	}

	void ApplicationLayer::OnEvent(Event &event) {

	}

#ifdef MGN_IMGUI
	void ApplicationLayer::ImGuiChaikin() {
		ImGui::Begin("Chaikin Curve");
		{
			{
				auto u = m_ChaikinCurves.GetU();
				if (ImGui::SliderFloat("U", &u, 0, 1)) {
					m_ChaikinCurves.SetU(u);
				}

				auto v = m_ChaikinCurves.GetV();
				if (ImGui::SliderFloat("V", &v, 0, 1)) {
					m_ChaikinCurves.SetV(v);
				}
			}
			{
				uint64_t steps = m_ChaikinCurves.GetStepCount();
				const uint64_t step = 1;
				const uint64_t step_fast = 100;
				if (ImGui::InputScalar("Steps", ImGuiDataType_U64, &steps, &step, &step_fast, "%ull")) {
					m_ChaikinCurves.SetStepCount(steps);
				}
			}
			{
				if (ImGui::Button("Clear")) {
					m_ChaikinCurves.ClearPoints();
				}
				static Vec3 p{};
				ImGui::DragFloat3("##NewPoint", Math::ValuePtr(p));
				ImGui::SameLine();
				if (ImGui::Button("Add")) {
					m_ChaikinCurves.AddPoint(p);
				}
			}
			{
				for (uint64_t i = 0; i < m_ChaikinCurves.line.size(); ++i) {
					auto& p = m_ChaikinCurves.line[i];
					std::string label{"Point " + std::to_string(i)};
					ImGui::PushID(i);
					ImGui::DragFloat3(label.c_str(), Math::ValuePtr(p), 0.01);
					ImGui::PopID();
				}
			}

			ImGui::BeginDisabled(m_ChaikinCurves.line.empty());
			if (ImGui::Button("Create Curve")) {
				const auto vecLine = m_ChaikinCurves.CalculateChaikin();
				m_Line.points.clear();
				m_Line.points.reserve(vecLine.size() + 1);
				for (const auto &p: vecLine) {
					Vertex v{};
					v.position = p;
					v.color = {0.8, 0.3, 0.2, 1.0};
					m_Line.points.push_back(v);
				}
				m_Line.points.push_back(m_Line.points.front());
			}
			ImGui::EndDisabled();

			if (ImGui::Button("Delete Curve")) {
				m_Line.points.clear();
			}
		}
		ImGui::End();
	}
	void ApplicationLayer::ImGuiLoop() {
		ImGui::Begin("Loop Subdivide");
		{
			static std::string s_MeshPath;

		}
		ImGui::End();
	}
	void ApplicationLayer::ImGuiKobbelt() {
	}
#else
	void ApplicationLayer::ImGuiChaikin() {}
	void ApplicationLayer::ImGuiLoop() {}
	void ApplicationLayer::ImGuiKobbelt() {}
#endif
} // namespace Imagine::Application
