//
// Created by Sayama on 29/04/2025.
//

#include "Imagine/Application/Application.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/SparseSet.hpp"
#include "Imagine/Scene/Renderable.hpp"
#include "Imagine/Scene/Scene.hpp"
#include "Imagine/Scene/SceneManager.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#include "Imagine/Rendering/MgnImGui.hpp"
#endif
namespace Imagine::Core {

	class TmpElement {
	public:
		inline static uint64_t ElementsCount{0};
		TmpElement() {
			ElementsCount += 1;
			MGN_LOG_DEBUG("Create Tmp Element. Element Count = {}", ElementsCount);
		}
		~TmpElement() {
			ElementsCount -= 1;
			MGN_LOG_DEBUG("Delete Tmp Element. Element Count = {}", ElementsCount);
		}
	};


	Application *Application::s_Instance{nullptr};

	Application *Application::Create(const ApplicationParameters &parameters) {
		Application *application{nullptr};
		application = new Application(parameters);
		return application;
	}

	Application *Application::Initialize(const ApplicationParameters &parameters) {
		if (s_Instance) return s_Instance;
		s_Instance = Application::Create(parameters);
		return s_Instance;
	}

	void Application::Shutdown() {
		delete s_Instance;
		s_Instance = nullptr;
	}

	Application *Application::Get() {
		return s_Instance;
	}

	Application::Application(const ApplicationParameters &parameters) :
		m_Parameters(parameters) {
		m_ShouldStop = false;
		m_LastFrame = m_Start = std::chrono::high_resolution_clock::now();
		m_DeltaTime = 0.01666666f;

		if (parameters.Window) {
			m_Window = Window::Initialize(parameters.AppName, parameters.Window.value());
		}

		if (parameters.Renderer) {
			m_Renderer = Renderer::Initialize(parameters);
		}
#ifdef MGN_IMGUI
		MgnImGui::CreateContext();
		MgnImGui::InitializeWindow();
		MgnImGui::InitializeRenderer();
#endif
	}

	Application::~Application() {
		SceneManager::Shutdown();

		if (m_Renderer) {
			m_Renderer->PrepareShutdown();
		}

#ifdef MGN_IMGUI
		MgnImGui::ShutdownRenderer();
		MgnImGui::ShutdownWindow();
		MgnImGui::Shutdown();
#endif

		if (m_Renderer) {
			m_Renderer = nullptr;
			Renderer::Shutdown();
		}

		if (m_Window) {
			m_Window = nullptr;
			Window::Shutdown();
		}
	}

	void Application::Stop() {
		m_ShouldStop = true;
	}

	void Application::Run() {
		while (!m_ShouldStop) {
			bool canDraw = true;
			if (m_Window) {
				m_Window->Update();
				m_ShouldStop |= m_Window->ShouldClose();
				canDraw = !m_Window->IsMinimized();
			}

			if (canDraw) {
#ifdef MGN_IMGUI
				MgnImGui::NewRenderFrame();
				MgnImGui::NewWindowFrame();
				MgnImGui::NewFrame();

				static bool open = true;
				ImGui::ShowDemoWindow(&open);

				if (m_Window) {
					m_Window->SendImGuiCommands();
				}

				if (m_Renderer) {
					m_Renderer->SendImGuiCommands();
				}

				// TODO: Other imgui rendering functions

				MgnImGui::Render();
#endif
			}

			if (m_Renderer && canDraw) {
				if (m_Renderer->BeginDraw()) {
					m_Renderer->Draw();

					m_Renderer->EndDraw();
					m_Renderer->Present();
				}
			}

			std::chrono::high_resolution_clock::time_point newFrame = std::chrono::high_resolution_clock::now();
			m_DeltaTime = std::chrono::duration<double, std::chrono::seconds::period>(newFrame - m_LastFrame).count();
			m_LastFrame = newFrame;

			// MGN_CORE_INFO("Frame #{}", m_CurrentFrame);
			// MGN_CORE_INFO("DeltaTime #{}", m_DeltaTime);
			// MGN_CORE_INFO("Time #{}", Time());

			m_CurrentFrame += 1;
		}
	}

	double Application::Time() const {
		std::chrono::high_resolution_clock::time_point newFrame = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double, std::chrono::seconds::period>(newFrame - m_Start).count();
	}
} // namespace Imagine::Core
