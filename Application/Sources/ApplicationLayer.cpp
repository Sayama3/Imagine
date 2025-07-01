//
// Created by Iannis on 24/06/2025.
//

#include "Imagine/ApplicationLayer.hpp"

#include "Imagine/Core/Inputs.hpp"
#include "Imagine/Events/ApplicationEvent.hpp"
#include "Imagine/Events/KeyEvent.hpp"
#include "Imagine/Events/MouseEvent.hpp"
#include "Imagine/Rendering/Camera.hpp"
#include "Imagine/Scene/SceneManager.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#endif

using namespace Imagine::Core;

namespace Imagine::Application {

	ApplicationLayer::ApplicationLayer() {
		m_MeshGraph = new Math::MeshGraph3D();
	}
	ApplicationLayer::~ApplicationLayer() {
		delete m_MeshGraph;
	}
	void ApplicationLayer::OnAttach() {
		m_Window = Window::Get();
		m_Renderer = Renderer::Get();

		m_Mesh = CPUMesh::LoadExternalModelAsMesh("Assets/Models/Box.glb");

		m_OriginalMeshEntityID = SceneManager::GetMainScene()->CreateEntity();
		SceneManager::GetMainScene()->GetEntity(m_OriginalMeshEntityID).LocalPosition = {-1, 0, 0};

		m_Renderer->LoadCPUMeshInScene(m_Mesh, SceneManager::GetMainScene().get(), m_OriginalMeshEntityID);

		m_LoopMeshEntityID = SceneManager::GetMainScene()->CreateEntity();
		SceneManager::GetMainScene()->GetEntity(m_LoopMeshEntityID).LocalPosition = {1, 0, 0};

		m_MeshGraph->Clear();
		m_MeshGraph->AddMesh(m_Mesh);

		m_MeshGraph->EnsureLink();
		m_MeshChanged = false;

		m_ChaikinCurves.SetUV(0.3, 0.3);
	}

	void ApplicationLayer::OnDetach() {
		m_MeshGraph->Clear();
	}

	void ApplicationLayer::OnUpdate(Core::TimeStep timeStep) {
		if (m_MeshChanged) {
			m_MeshGraph->Clear();
			m_MeshGraph->AddMesh(m_Mesh);
			m_MeshChanged = false;
		}
	}

	void ApplicationLayer::OnImGuiRender() {
		ImGuiChaikin();

		ImGuiSubdivide();
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

		EventDispatcher dispatch{event};

		bool handled = dispatch.Dispatch<AppUpdateEvent>([this](AppUpdateEvent& event) {
			OnUpdate(event.GetTimeStep());
			return false;
		});

		if(handled) return;
		handled = dispatch.Dispatch<ImGuiEvent>([this](ImGuiEvent& event) {
			OnImGuiRender();
			return false;
		});

		if(handled) return;
		handled = dispatch.Dispatch<AppRenderEvent>([this](AppRenderEvent& event) {
			Core::DrawContext ctx;
			OnRender(ctx);
			Renderer::Get()->Draw(ctx);
			return false;
		});

		if(handled) return;
		handled = dispatch.Dispatch<MouseMovedEvent>([this](MouseMovedEvent &mouse) -> bool {
			MoveMouse({mouse.GetX(), mouse.GetY()});
			return false;
		});

		if(handled) return;
		handled = dispatch.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent &mouse) -> bool {
			if (mouse.GetMouseButton() == Imagine::Mouse::Right) {
				const Rect<> window = m_Window->GetWindowRect();
				const Rect<> viewport = m_Renderer->GetViewport();
				const Vec2 globalPos = m_MousePos + window.min;
				const Vec2 viewportPos = globalPos - viewport.min;

				const Vec3 camPos = Camera::s_MainCamera->position;
				const Vec3 worldPos = m_Renderer->GetWorldPoint(viewportPos);
				const Vec3 fwd = Camera::s_MainCamera->GetForward();
				const auto ray = Ray3{camPos, Math::Normalize(worldPos - camPos)};

				const std::optional<Vec3> mouseOnGround = Math::RaycastToPoint(Plane{Vec3{0, 0, 0}, Vec3{0, 1, 0}}, ray);
				if (mouseOnGround && SceneManager::GetMainScene()->Exist(m_OriginalMeshEntityID)) {
					m_ChaikinCurves.AddPoint(mouseOnGround.value());
					return true;
				}
			}
			return false;
		});

		if(handled) return;
		handled = dispatch.Dispatch<WindowDropFileEvent>([this](WindowDropFileEvent &e) -> bool {
			for (const std::filesystem::path &path: e) {
				if (std::filesystem::exists(path)) {
					Renderer::Get()->LoadExternalModelInScene(path, SceneManager::GetMainScene().get());
				}
			}
			return false;
		});

		// Handle Camera movement.
		if(handled) return;
		handled = dispatch.Dispatch<KeyPressedEvent>([](KeyPressedEvent &e) -> bool {
			if (e.GetKeyCode() == Imagine::Key::W) {
				Core::Camera::s_MainCamera->velocity.z = +1;
			}
			if (e.GetKeyCode() == Imagine::Key::S) {
				Core::Camera::s_MainCamera->velocity.z = -1;
			}
			if (e.GetKeyCode() == Imagine::Key::A) {
				Core::Camera::s_MainCamera->velocity.x = -1;
			}
			if (e.GetKeyCode() == Imagine::Key::D) {
				Core::Camera::s_MainCamera->velocity.x = +1;
			}
			if (e.GetKeyCode() == Imagine::Key::E) {
				Core::Camera::s_MainCamera->velocity.y = +1;
			}
			if (e.GetKeyCode() == Imagine::Key::Q) {
				Core::Camera::s_MainCamera->velocity.y = -1;
			}

			if (e.GetKeyCode() == Imagine::Key::Right) {
				Core::Camera::s_MainCamera->yawVelocity = +45;
			}
			if (e.GetKeyCode() == Imagine::Key::Left) {
				Core::Camera::s_MainCamera->yawVelocity = -45;
			}

			if (e.GetKeyCode() == Imagine::Key::Up) {
				Core::Camera::s_MainCamera->pitchVelocity = +45;
			}
			if (e.GetKeyCode() == Imagine::Key::Down) {
				Core::Camera::s_MainCamera->pitchVelocity = -45;
			}

			return true;
		});

		// Handle Camera movement.
		if(handled) return;
		dispatch.Dispatch<KeyReleasedEvent>([](const KeyReleasedEvent &e) -> bool {
			if (e.GetKeyCode() == Imagine::Key::W) {
				Core::Camera::s_MainCamera->velocity.z = 0;
			}
			if (e.GetKeyCode() == Imagine::Key::S) {
				Core::Camera::s_MainCamera->velocity.z = 0;
			}
			if (e.GetKeyCode() == Imagine::Key::A) {
				Core::Camera::s_MainCamera->velocity.x = 0;
			}
			if (e.GetKeyCode() == Imagine::Key::D) {
				Core::Camera::s_MainCamera->velocity.x = 0;
			}
			if (e.GetKeyCode() == Imagine::Key::E) {
				Core::Camera::s_MainCamera->velocity.y = 0;
			}
			if (e.GetKeyCode() == Imagine::Key::Q) {
				Core::Camera::s_MainCamera->velocity.y = 0;
			}

			if (e.GetKeyCode() == Imagine::Key::Right) {
				Core::Camera::s_MainCamera->yawVelocity = 0;
			}
			if (e.GetKeyCode() == Imagine::Key::Left) {
				Core::Camera::s_MainCamera->yawVelocity = 0;
			}

			if (e.GetKeyCode() == Imagine::Key::Up) {
				Core::Camera::s_MainCamera->pitchVelocity = 0;
			}
			if (e.GetKeyCode() == Imagine::Key::Down) {
				Core::Camera::s_MainCamera->pitchVelocity = 0;
			}

			return true;
		});
	}

	void ApplicationLayer::MoveMouse(Vec2 pos) {
		m_MousePos = pos;
	}

	bool ApplicationLayer::ChangeModelAndPath(const std::filesystem::path &path) {
		std::error_code e;
		if (std::filesystem::equivalent(path, m_ModelPath, e)) return true;

		const auto mesh = CPUMesh::LoadExternalModelAsMesh(path);
		if (mesh.Vertices.empty()) {
			return false;
		}

		m_ModelPath = path;
		m_Mesh = std::move(mesh);

		m_Renderer->LoadCPUMeshInScene(m_Mesh, SceneManager::GetMainScene().get(), m_OriginalMeshEntityID);
		m_MeshChanged = true;

		return true;
	}

#ifdef MGN_IMGUI
	void ApplicationLayer::ImGuiChaikin() {

		ImGui::SetNextWindowSize({200, 400}, ImGuiCond_FirstUseEver);
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
				int steps = static_cast<int>(m_ChaikinCurves.GetStepCount());
				if (ImGui::InputInt("Steps", &steps)) {
					steps = std::max(steps, 1);
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
					auto &p = m_ChaikinCurves.line[i];
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

	void ApplicationLayer::ImGuiSubdivide() {

		ImGui::SetNextWindowSize({200, 400}, ImGuiCond_FirstUseEver);
		ImGui::Begin("Subdivisions");
		{
			static int s_Step = 1;
			static bool s_Smooth = false;
			static bool s_ResetMeshAfterSubdivision = true;
			static bool s_RandomColor = true;
			std::string modelPath = m_ModelPath.string();
			if (ImGui::InputText("Model Path", &modelPath)) m_ModelPath = modelPath;
			if (ImGui::Button("Reload")) {
				ChangeModelAndPath(modelPath);
			}

			ImGui::InputInt("Step Count", &s_Step, 1, 10, 0);
			s_Step = std::max(s_Step, 0);


			ImGui::Checkbox("Shade Smooth", &s_Smooth);
			if (ImGui::Checkbox("Reset Mesh Graph After Subdivision", &s_ResetMeshAfterSubdivision) && s_ResetMeshAfterSubdivision) {
				m_MeshChanged = true;
			}

			ImGui::BeginDisabled(s_Smooth);
			ImGui::Checkbox("Random Color", &s_RandomColor);
			ImGui::EndDisabled();

			ImGui::BeginDisabled(m_MeshChanged);
			if (ImGui::Button("Loop Subdivide")) {
				long double total{0};
				for (uint32_t i = 0; i < s_Step; ++i) {
					const auto before = std::chrono::high_resolution_clock::now();
					m_MeshGraph->SubdivideLoop();
					const auto after = std::chrono::high_resolution_clock::now();
					const auto ms = std::chrono::duration_cast<std::chrono::duration<long double, std::milli>>(after - before).count();
					total += ms;
					MGN_INFO("Subdivision {} took {}ms", i + 1, ms);
					// m_MeshGraph->EnsureLink();
				}
				MGN_INFO("Total of {} subdivision took {}ms", s_Step, total);

				if (s_Smooth) {
					const auto before = std::chrono::high_resolution_clock::now();
					m_SubdividedMesh = m_MeshGraph->GetSmoothCPUMesh();
					const auto after = std::chrono::high_resolution_clock::now();
					const auto ms = std::chrono::duration_cast<std::chrono::duration<long double, std::milli>>(after - before).count();
					MGN_INFO("Creating a smooth mesh took {}ms", ms);
				}
				else {
					const auto before = std::chrono::high_resolution_clock::now();
					if (s_RandomColor)
						m_SubdividedMesh = m_MeshGraph->GetHardCPUMesh<true>();
					else
						m_SubdividedMesh = m_MeshGraph->GetHardCPUMesh<false>();
					const auto after = std::chrono::high_resolution_clock::now();
					const auto ms = std::chrono::duration_cast<std::chrono::duration<long double, std::milli>>(after - before).count();
					MGN_INFO("Creating a hard mesh took {}ms", ms);
				}

				m_Renderer->LoadCPUMeshInScene(m_SubdividedMesh, SceneManager::GetMainScene().get(), m_LoopMeshEntityID);
				if (s_ResetMeshAfterSubdivision) m_MeshChanged = true;
			}

			if (ImGui::Button("Butterfly Subdivide")) {
				long double total{0};
				for (uint32_t i = 0; i < s_Step; ++i) {
					const auto before = std::chrono::high_resolution_clock::now();
					m_MeshGraph->SubdivideButterfly();
					const auto after = std::chrono::high_resolution_clock::now();
					const auto ms = std::chrono::duration_cast<std::chrono::duration<long double, std::milli>>(after - before).count();
					total += ms;
					MGN_INFO("Subdivision {} took {}ms", i + 1, ms);
					// m_MeshGraph->EnsureLink();
				}
				MGN_INFO("Total of {} subdivision took {}ms", s_Step, total);

				if (s_Smooth) {
					const auto before = std::chrono::high_resolution_clock::now();
					m_SubdividedMesh = m_MeshGraph->GetSmoothCPUMesh();
					const auto after = std::chrono::high_resolution_clock::now();
					const auto ms = std::chrono::duration_cast<std::chrono::duration<long double, std::milli>>(after - before).count();
					MGN_INFO("Creating a smooth mesh took {}ms", ms);
				}
				else {
					const auto before = std::chrono::high_resolution_clock::now();
					if (s_RandomColor)
						m_SubdividedMesh = m_MeshGraph->GetHardCPUMesh<true>();
					else
						m_SubdividedMesh = m_MeshGraph->GetHardCPUMesh<false>();
					const auto after = std::chrono::high_resolution_clock::now();
					const auto ms = std::chrono::duration_cast<std::chrono::duration<long double, std::milli>>(after - before).count();
					MGN_INFO("Creating a hard mesh took {}ms", ms);
				}

				m_Renderer->LoadCPUMeshInScene(m_SubdividedMesh, SceneManager::GetMainScene().get(), m_LoopMeshEntityID);
				if (s_ResetMeshAfterSubdivision) m_MeshChanged = true;
			}

			if (ImGui::Button("Kobbelt Subdivide")) {
				long double total{0};
				for (uint32_t i = 0; i < s_Step; ++i) {
					const auto before = std::chrono::high_resolution_clock::now();
					m_MeshGraph->SubdivideKobbelt();
					const auto after = std::chrono::high_resolution_clock::now();
					const auto ms = std::chrono::duration_cast<std::chrono::duration<long double, std::milli>>(after - before).count();
					total += ms;
					MGN_INFO("Subdivision {} took {}ms", i + 1, ms);
					// m_MeshGraph->EnsureLink();
				}
				MGN_INFO("Total of {} subdivision took {}ms", s_Step, total);

				if (s_Smooth) {
					const auto before = std::chrono::high_resolution_clock::now();
					m_SubdividedMesh = m_MeshGraph->GetSmoothCPUMesh();
					const auto after = std::chrono::high_resolution_clock::now();
					const auto ms = std::chrono::duration_cast<std::chrono::duration<long double, std::milli>>(after - before).count();
					MGN_INFO("Creating a smooth mesh took {}ms", ms);
				}
				else {
					const auto before = std::chrono::high_resolution_clock::now();
					if (s_RandomColor)
						m_SubdividedMesh = m_MeshGraph->GetHardCPUMesh<true>();
					else
						m_SubdividedMesh = m_MeshGraph->GetHardCPUMesh<false>();
					const auto after = std::chrono::high_resolution_clock::now();
					const auto ms = std::chrono::duration_cast<std::chrono::duration<long double, std::milli>>(after - before).count();
					MGN_INFO("Creating a hard mesh took {}ms", ms);
				}

				m_Renderer->LoadCPUMeshInScene(m_SubdividedMesh, SceneManager::GetMainScene().get(), m_LoopMeshEntityID);
				if (s_ResetMeshAfterSubdivision) m_MeshChanged = true;
			}
			ImGui::EndDisabled();
		}
		ImGui::End();
	}

#else
	void ApplicationLayer::ImGuiChaikin() {}
	void ApplicationLayer::ImGuiSubdivide() {}
#endif
} // namespace Imagine::Application
