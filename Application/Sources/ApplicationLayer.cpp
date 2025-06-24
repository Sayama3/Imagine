//
// Created by Iannis on 24/06/2025.
//

#include "Imagine/ApplicationLayer.hpp"

#include "Imagine/Core/Inputs.hpp"
#include "Imagine/Rendering/Camera.hpp"
#include "Imagine/Scene/SceneManager.hpp"

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
				SceneManager::GetMainScene()->GetEntity(m_CubeEntityID).LocalPosition = mouseOnGround.value();
			}
		}
	}

	void ApplicationLayer::OnImGuiRender() {
	}

	void ApplicationLayer::OnRender(Core::DrawContext &ctx) {
		Vertex v1{};
		v1.position = {0, 0, 0};
		Vertex v2{};
		v2.position = {0, 1, 0};
		LineObject line;
		line.width = 5;
		line.points.push_back(v1);
		line.points.push_back(v2);

		ctx.OpaqueLines.push_back(line);
	}
} // namespace Imagine::Application
