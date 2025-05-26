//
// Created by Sayama on 29/04/2025.
//

#include "Imagine/Application/Application.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/SparseSet.hpp"
#include "Imagine/Scene/Renderable.hpp"
#include "Imagine/Scene/Scene.hpp"


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


	Application* Application::s_Instance{nullptr};

	Application* Application::Get()
	{
		return s_Instance;
	}

	Application::Application(const ApplicationParameters& parameters) : m_Parameters(parameters) {
		m_ShouldStop = false;
		m_LastFrame = m_Start = std::chrono::high_resolution_clock::now();
		m_DeltaTime = 0.01666666f;

		if (parameters.Window) {
			m_Window = Window::Initialize(parameters.AppName, parameters.Window.value());
		}

		if (parameters.UseRenderer) {
			m_Renderer = Renderer::Initialize(RendererParameters{},parameters);
		}

		if (!s_Instance)
		{
			s_Instance = this;
		}
	}

	Application::~Application() {
		if (m_Parameters.UseRenderer) {
			m_Renderer = nullptr;
			Renderer::Shutdown();
		}

		if (m_Parameters.Window) {
			m_Window = nullptr;
			Window::Shutdown();
		}

		if (s_Instance == this)
		{
			s_Instance = nullptr;
		}
	}

	void Application::Stop() {
		m_ShouldStop = true;
	}

	void Application::Run() {
		while (!m_ShouldStop) {
			if (m_Window) {
				m_Window->Update();
				m_ShouldStop |= m_Window->ShouldClose();
			}

			if (m_Renderer)
			{
				m_Renderer->Draw();
			}

			std::chrono::high_resolution_clock::time_point newFrame = std::chrono::high_resolution_clock::now();
			m_DeltaTime = std::chrono::duration<double, std::chrono::seconds::period>(newFrame - m_LastFrame).count();
			m_LastFrame = newFrame;

			MGN_CORE_INFO("Frame #{}", m_CurrentFrame);
			MGN_CORE_INFO("DeltaTime #{}", m_DeltaTime);
			MGN_CORE_INFO("Time #{}", Time());

			m_CurrentFrame += 1;
		}
	}

	double Application::Time() const {
		std::chrono::high_resolution_clock::time_point newFrame = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double, std::chrono::seconds::period>(newFrame - m_Start).count();
	}
} // Imagine