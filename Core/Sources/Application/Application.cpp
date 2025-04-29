//
// Created by Sayama on 29/04/2025.
//

#include "../../Includes/Imagine/Application/Application.hpp"
#include "Imagine/Core/Macros.hpp"

namespace Imagine::Core {
	Application::Application(ApplicationParameters parameters) : m_Parameters(std::move(parameters)) {
		m_ShouldStop = false;
		m_LastFrame = m_Start = std::chrono::high_resolution_clock::now();
		m_DeltaTime = 0.01666666f;
	}

	Application::~Application() {
	}

	void Application::Stop() {
		m_ShouldStop = true;
	}

	void Application::Run() {
		while (!m_ShouldStop) {
			std::chrono::high_resolution_clock::time_point newFrame = std::chrono::high_resolution_clock::now();
			m_DeltaTime = std::chrono::duration<double, std::chrono::seconds::period>(newFrame - m_LastFrame).count();
			m_LastFrame = newFrame;
			MGN_CORE_INFO("Frame #{}", m_CurrentFrame);
			MGN_CORE_INFO("DeltaTime #{}", m_DeltaTime);
			MGN_CORE_INFO("Time #{}", Time());
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			m_CurrentFrame += 1;
		}
	}

	double Application::Time() const {
		std::chrono::high_resolution_clock::time_point newFrame = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double, std::chrono::seconds::period>(newFrame - m_Start).count();
	}
} // Imagine